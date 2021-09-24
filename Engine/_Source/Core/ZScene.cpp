/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZScene.cpp

 Created by Adrian Sanchez on 19/04/19.
 Copyright © 2019 Pervasive Sense. All rights reserved.

 This file is part of Zenith.

 Zenith is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zenith is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ZScene.hpp"
#include "ZServices.hpp"
#include "ZRenderer.hpp"
#include "ZFramebuffer.hpp"
#include "ZGame.hpp"
#include "ZLight.hpp"
#include "ZSkybox.hpp"
#include "ZCamera.hpp"
#include "ZGrass.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZDomain.hpp"
#include "ZUIText.hpp"
#include "ZUICanvas.hpp"
#include "ZUIElement.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZSceneRoot.hpp"
#include "ZComponent.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZSceneReadyEvent.hpp"
#include "ZObjectDestroyedEvent.hpp"
#include "ZRaycastEvent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZWindowResizeEvent.hpp"
#include "ZStringHelpers.hpp"
#include "ZAssetLoadProgressTracker.hpp"

ZScene::ZScene(const std::string& name) : name_(name), playState_(ZPlayState::Loading)
{ }

ZScene::ZScene(std::initializer_list<std::string> zofPaths) : ZScene()
{
// Make sure scene description paths are cached before loading them asynchronously.
// This allows us to check if one of our ZOF files was loaded when receiving events
    for (std::string path : zofPaths)
    {
        pendingSceneDefinitions_[path] = true;
    }
    playState_ = ZPlayState::NotStarted;
}

void ZScene::Initialize()
{
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleWindowResize);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleZOFReady);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleObjectDestroyed);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleRaycastEvent);

    CreateSceneRoot(name_);
    CreateUICanvas();

    bvh_ = std::make_shared<ZBVH>(4, ZBVHSplitMethod::SAH);
    loadProgressTracker_ = std::make_shared<ZAssetLoadProgressTracker>();

    for (auto it = pendingSceneDefinitions_.begin(); it != pendingSceneDefinitions_.end(); it++)
    {
        ZServices::LoadZOF(it->first);
    }

    SetupRenderers();
    SetupRenderPasses();

    ZProcess::Initialize();
}

void ZScene::SetupRenderers()
{
    renderer_ = std::make_shared<ZRenderer>(shared_from_this());
    if (gameConfig_.domain.offline) {
        SetupTargetDrawBuffer();
    }
}

void ZScene::SetupTargetDrawBuffer()
{
    targetBuffer_ = ZFramebuffer::CreateColor(gameSystems_.domain->Resolution());
    renderer_->SetTarget(targetBuffer_);
}

void ZScene::SetupRenderPasses()
{
    // TODO: Possible performance penalty here. Color and depth information might be better computed in 
    // a single render pass using multiple render targets (i.e. G-Buffer pass)
    renderer_->AddPass(ZRenderPass::Depth());

    renderer_->AddPass(ZRenderPass::Shadow());

    renderer_->AddPass(ZRenderPass::Color(gameSystems_.domain->Resolution()));

    renderer_->AddPass(ZRenderPass::Post(gameSystems_.domain->Resolution()));

    renderer_->AddPass(ZRenderPass::UI(gameSystems_.domain->Resolution()));
}

void ZScene::Update(double deltaTime)
{
    if (playState_ == ZPlayState::Playing || playState_ == ZPlayState::Paused)
    {
        root_->Prepare(deltaTime);
        canvas_->Prepare(deltaTime);
        renderer_->Render(deltaTime);
        bvh_->Build();
    }
    else if (playState_ == ZPlayState::Loading)
    {
        if (loadProgressTracker_->Completed())
        {
			playState_ = ZPlayState::Ready;
			std::shared_ptr<ZSceneReadyEvent> sceneReadyEvent = std::make_shared<ZSceneReadyEvent>(shared_from_this());
			ZServices::EventAgent()->Queue(sceneReadyEvent);
        }
    }
}

void ZScene::Play()
{
    playState_ = ZPlayState::Playing;
    gameSystems_.physics->Resume();
    if (activeCamera_) {
        activeCamera_->EnableLook();
        activeCamera_->EnableMovement();
    }
}

void ZScene::Pause()
{
    playState_ = ZPlayState::Paused;
    gameSystems_.physics->Pause();
    if (activeCamera_) {
        activeCamera_->DisableLook();
        activeCamera_->DisableMovement();
    }
}

void ZScene::Stop()
{
    playState_ = ZPlayState::Ready;
    gameSystems_.physics->Pause();
    if (activeCamera_) {
        activeCamera_->DisableLook();
        activeCamera_->DisableMovement();
    }
}

void ZScene::UpdateLightspaceMatrices()
{
    if (gameLights_.empty() || !activeCamera_->Moving()) return;
    ZFrustum frustum = activeCamera_->Frustum();
    for (const auto& light : gameLights_) {
        light->UpdateLightspaceMatrices(frustum);
    }
}

ZSceneSnapshot ZScene::Snapshot()
{
    ZSceneSnapshot snapshot;
    std::shared_ptr<ZScene> sceneClone = std::make_shared<ZScene>(name_);

    sceneClone->playState_ = playState_;
    sceneClone->state_ = state_;

    sceneClone->AddGameObject(skybox_->Clone(), false);
    for (const auto& obj : gameObjects_)
        sceneClone->AddGameObject(obj->Clone(), false);

    for (const auto& el : uiElements_)
        sceneClone->AddUIElement(el); // TODO: Clone UI Elements too

    snapshot.scene = sceneClone;
    snapshot.time = SECONDS_TIME;

    return snapshot;
}

void ZScene::RestoreSnapshot(ZSceneSnapshot& snapshot)
{
    CreateSceneRoot(name_);

    playState_ = snapshot.scene->playState_;
    state_ = snapshot.scene->state_;

    while (!gameObjects_.empty())
        (*gameObjects_.begin())->Destroy();

    gameObjects_.clear(); uiElements_.clear(); gameLights_.clear();
    gameObjectIDMap_.clear(); uiElementIDMap_.clear(); gameLightIDMap_.clear();

    AddGameObject(snapshot.scene->skybox_);
    for (const auto& obj : snapshot.scene->gameObjects_)
        AddGameObject(obj);

    for (const auto& el : snapshot.scene->uiElements_)
        AddUIElement(el);
}

void ZScene::AddBVHPrimitive(const ZBVHPrimitive& primitive)
{
    bvh_->AddPrimitive(primitive);
}

void ZScene::CreateSceneRoot(const std::string& name)
{
    root_ = ZSceneRoot::Create();
    root_->SetName(name);
    root_->SetScene(shared_from_this());
    root_->Initialize();
}

void ZScene::CreateUICanvas()
{
    ZUIElementOptions elementOptions;
    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    elementOptions.color = glm::vec4(1.f);
    canvas_ = ZUICanvas::Create(elementOptions, shared_from_this());
}

void ZScene::AddGameObjects(std::initializer_list<std::shared_ptr<ZGameObject>> gameObjects, bool runImmediately)
{
    for (std::shared_ptr<ZGameObject> object : gameObjects)
    {
        AddGameObject(object, runImmediately);
    }
}

void ZScene::AddGameObject(std::shared_ptr<ZGameObject> gameObject, bool runImmediately)
{
    if (gameObject != nullptr)
    {
        gameObject->SetScene(shared_from_this());
        if (auto camera = std::dynamic_pointer_cast<ZCamera>(gameObject))
        {
            if (camera->IsPrimary()) primaryCamera_ = camera;
            activeCamera_ = camera;
        } 
        else if (auto light = std::dynamic_pointer_cast<ZLight>(gameObject))
        {
            if (gameLightIDMap_.find(gameObject->ID()) == gameLightIDMap_.end()) {
                gameLightIDMap_[gameObject->ID()] = gameLights_.size();
                gameLights_.push_back(light);
            }
        }

        if (auto skybox = std::dynamic_pointer_cast<ZSkybox>(gameObject))
        {
            skybox_ = skybox;
        }
        else
        {
            if (gameObjectIDMap_.find(gameObject->ID()) == gameObjectIDMap_.end()) {
                gameObjectIDMap_[gameObject->ID()] = gameObjects_.size();
                gameObjects_.push_back(gameObject);
            }
        }

        if (!gameObject->Parent())
            root_->AddChild(gameObject);

        if (runImmediately) {
            for (auto comp : gameObject->components_)
                ZServices::ProcessRunner(gameName_)->AttachProcess(comp);
        }

        for (auto child : gameObject->Children()) {
            AddGameObject(child);
        }
    }
}

void ZScene::RemoveGameObject(std::shared_ptr<ZGameObject> gameObject)
{
    if (gameObject != nullptr)
    {
        if (primaryCamera_ == gameObject)
        {
            primaryCamera_ = nullptr;
        }
        else if (activeCamera_ == gameObject)
        {
            activeCamera_ = nullptr;
        }

        auto objectID = gameObject->ID();
        if (gameLightIDMap_.find(objectID) != gameLightIDMap_.end()) {
            gameLights_.erase(gameLights_.begin() + gameLightIDMap_[objectID]);
            gameLightIDMap_.erase(objectID);
        }
        if (gameObjectIDMap_.find(objectID) != gameObjectIDMap_.end()) {
            gameObjects_.erase(gameObjects_.begin() + gameObjectIDMap_[objectID]);
            gameObjectIDMap_.erase(objectID);
        }

        if (auto parent = gameObject->Parent()) {
            if (parent == root_)
                root_->RemoveChild(gameObject, true);
        }
        gameObject->parent_.reset();
        gameObject->SetScene(nullptr);

        for (const auto& child : gameObject->Children()) {
            RemoveGameObject(child);
        }
    }
}

std::shared_ptr<ZGameObject> ZScene::FindGameObject(const std::string& id)
{
    if (!id.empty())
    {
        for (const auto& obj : gameObjects_)
        {
            if (zenith::strings::HasSuffix(obj->ID(), id))
            {
                return obj;
            }
            else if (obj->HasChildren())
            {
                const auto& found = obj->Child<ZGameObject>(id);
                if (found) return found;
            }
        }
    }
    return nullptr;
}

void ZScene::AddUIElement(std::shared_ptr<ZUIElement> element)
{
    if (element && uiElementIDMap_.find(element->ID()) == uiElementIDMap_.end())
    {
        canvas_->AddChild(element);
        uiElementIDMap_[element->ID()] = uiElements_.size();
        uiElements_.push_back(element);
    }
}

void ZScene::AddUIElements(std::initializer_list<std::shared_ptr<ZUIElement>> elements)
{
    for (std::shared_ptr<ZUIElement> element : elements)
    {
        AddUIElement(element);
    }
}

void ZScene::RemoveUIElement(std::shared_ptr<ZUIElement> element)
{
    auto id = element->ID();
    if (element && uiElementIDMap_.find(id) != uiElementIDMap_.end())
    {
        uiElements_.erase(uiElements_.begin() + uiElementIDMap_[id]);
        uiElementIDMap_.erase(id);
        canvas_->RemoveChild(element, true);
        element->parent_.reset();
        element->scene_.reset();
    }
}

std::shared_ptr<ZUIElement> ZScene::FindUIElement(const std::string& id)
{
    if (!id.empty())
    {
        for (const auto& el : uiElements_)
        {
            if (zenith::strings::HasSuffix(el->ID(), id))
            {
                return el;
            }
            else if (el->HasChildren())
            {
                const auto& found = el->Child<ZUIElement>(id);
                if (found) return found;
            }
        }
    }
    return nullptr;
}

ZRay ZScene::ScreenPointToWorldRay(const glm::vec2& point, const glm::vec2& dimensions)
{
    auto cam = ActiveCamera();
    if (!cam)
        return ZRay(glm::vec3(0.f), glm::vec3(0.f));

    auto rectRes = dimensions == glm::vec2(0.f) ? gameSystems_.domain->Resolution() : dimensions;
    float x = (2.f * point.x) / rectRes.x - 1.f;
    float y = (2.f * point.y) / rectRes.y - 1.f;

    glm::vec4 start = glm::normalize(cam->InverseViewProjectionMatrix() * glm::vec4(x, -y, 0.f, 1.f));
    start /= start.w;
    glm::vec4 end = glm::normalize(cam->InverseViewProjectionMatrix() * glm::vec4(x, -y, 1.f, 1.f));
    end /= end.w;
    glm::vec4 direction = glm::normalize(end - start);

    return ZRay(cam->Position(), direction);
}

bool ZScene::RayCast(ZRay& ray, ZIntersectHitResult& hitResult)
{
    return bvh_->Intersect(ray, hitResult);
}

ZHTexture ZScene::TargetTexture()
{
    if (targetBuffer_)
    {
        return targetBuffer_->BoundAttachment();
    }
    return ZHTexture();
}

void ZScene::SetDefaultSkybox()
{
    skybox_ = std::shared_ptr<ZSkybox>(new ZSkybox(DEFAULT_HDR_CUBEMAP));
    skybox_->InitializeAsync();
    AddGameObject(skybox_);
}

void ZScene::LoadSceneData(const std::shared_ptr<ZOFNode>& objectTree)
{
    ParseSceneMetadata(objectTree);

	for (ZOFChildList::iterator it = objectTree->children.begin(); it != objectTree->children.end(); it++)
	{
		std::shared_ptr<ZOFObjectNode> dataNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
        std::shared_ptr<ZGameObject> gameObject;
        switch (dataNode->type)
        {
            case ZOFObjectType::Script:
            {
                ZServices::ScriptManager()->DeserializeAsync(dataNode->id, dataNode);
                break;
            }
            case ZOFObjectType::Texture:
            {
                ZServices::FontManager()->DeserializeAsync(dataNode->id, dataNode);
                break;
            }
            case ZOFObjectType::Shader:
            {
		        ZServices::ShaderManager()->DeserializeAsync(dataNode->id, dataNode);
                break;
            }
            case ZOFObjectType::Model:
            {
                ZServices::ModelManager()->DeserializeAsync(dataNode->id, dataNode);
                break;
            }
            case ZOFObjectType::Material:
            {
                ZServices::MaterialManager()->DeserializeAsync(dataNode->id, dataNode);
                break;
            }
			case ZOFObjectType::GameObject:
            {
				gameObject = ZGameObject::Create(dataNode, shared_from_this());
				break;
            }
            case ZOFObjectType::Light:
            {
                gameObject = ZLight::Create(dataNode, shared_from_this());
                break;
            }
            case ZOFObjectType::Camera:
            {
                gameObject = ZCamera::Create(dataNode, shared_from_this());
                break;
            }
            case ZOFObjectType::Skybox:
            {
                gameObject = ZSkybox::Create(dataNode, shared_from_this());
                break;
            }
            case ZOFObjectType::Grass:
            {
                gameObject = ZGrass::Create(dataNode, shared_from_this());
                break;
            }
            default: break;
        }

		if (gameObject) {
			for (ZOFChildList::iterator compIt = dataNode->children.begin(); compIt != dataNode->children.end(); compIt++)
			{
				std::shared_ptr<ZOFObjectNode> componentNode = std::static_pointer_cast<ZOFObjectNode>(compIt->second);
				ZComponent::CreateIn(gameObject, componentNode);
			}
            AddGameObject(gameObject);
		}
    }

	ZOFLoadResult zofResults;
	zofResults.gameObjects = ZGameObject::Load(objectTree, shared_from_this());
    zofResults.uiElements = ZUIElement::Deserialize(objectTree, shared_from_this());

    for (ZUIElementList::iterator it = zofResults.uiElements.begin(); it != zofResults.uiElements.end(); it++)
    {
        AddUIElement(*it);
    }
}

void ZScene::ParseSceneMetadata(const std::shared_ptr<ZOFNode>& objectTree)
{
    bool hasSkybox = false;
    for (ZOFChildList::iterator it = objectTree->children.begin(); it != objectTree->children.end(); it++)
    {
        std::shared_ptr<ZOFObjectNode> dataNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
        if (dataNode->type == ZOFObjectType::Scene)
        {
            std::shared_ptr<ZOFObjectNode> sceneDataNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (sceneDataNode->properties.find("name") != sceneDataNode->properties.end())
            {
                name_ = sceneDataNode->properties["name"]->Value<ZOFString>(0)->value;
                root_->SetName(name_);
            }
		}
		else if (dataNode->type == ZOFObjectType::Texture)
        {
            loadProgressTracker_->TrackTexture(ZHTexture(it->first.value));
        }
		else if (dataNode->type == ZOFObjectType::Model)
		{
			loadProgressTracker_->TrackModel(ZHModel(it->first.value));
		}
		else if (dataNode->type == ZOFObjectType::Material)
		{
			loadProgressTracker_->TrackMaterial(ZHMaterial(it->first.value));
		}
		else if (dataNode->type == ZOFObjectType::Audio)
		{
			loadProgressTracker_->TrackAudio(ZHAudio(it->first.value));
		}
		else if (dataNode->type == ZOFObjectType::Shader)
		{
			loadProgressTracker_->TrackShader(ZHShader(it->first.value));
		}
		else if (dataNode->type == ZOFObjectType::Font)
		{
			loadProgressTracker_->TrackFont(ZHFont(it->first.value));
		}
		else if (dataNode->type == ZOFObjectType::Script)
		{
			loadProgressTracker_->TrackScript(ZHScript(it->first.value));
		}
        else if (dataNode->type == ZOFObjectType::Skybox)
        {
            hasSkybox = true;
        }
    }

    if (!hasSkybox && gameConfig_.graphics.clearColor == glm::vec4(0.f, 0.f, 0.f, 1.f))
    {
        SetDefaultSkybox();
    }
}

void ZScene::UnregisterLoadDelegates()
{
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleZOFReady);
}

void ZScene::HandleWindowResize(const std::shared_ptr<ZWindowResizeEvent>& event)
{
    glm::vec2 newResolution = event->Size();
    if (targetBuffer_) {
        targetBuffer_->Resize(newResolution.x, newResolution.y);
    }
    for (auto pass : renderer_->Passes()) {
        pass->SetSize(newResolution);
    }
}

void ZScene::HandleZOFReady(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (event->Resource() == nullptr)
    {
        return;
    }

    std::shared_ptr<ZZofResourceData> zofResource = std::static_pointer_cast<ZZofResourceData>(event->Resource());

    if (pendingSceneDefinitions_.find(zofResource->path) != pendingSceneDefinitions_.end())
    {
        LoadSceneData(zofResource->objectTree);
        pendingSceneDefinitions_.erase(zofResource->path);
        playState_ = ZPlayState::Loading;
    }

    if (pendingSceneDefinitions_.empty())
    {
        ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleZOFReady);
    }
}

void ZScene::HandleObjectDestroyed(const std::shared_ptr<ZObjectDestroyedEvent>& event)
{
    RemoveGameObject(event->Object());
}

void ZScene::HandleRaycastEvent(const std::shared_ptr<ZRaycastEvent>& event)
{
    ZRaycastHitResult hitResult = gameSystems_.physics->Raycast(event->Origin(), event->Direction(), event->Length());
    if (hitResult.hasHit)
    {
        // TODO: Rename to ZObjectRayHit event or something similar
        std::shared_ptr<ZObjectSelectedEvent> objectSelectEvent = std::make_shared<ZObjectSelectedEvent>(hitResult.objectHit->ID(), hitResult.hitPosition);
        ZServices::EventAgent()->Trigger(objectSelectEvent);
    }
}

void ZScene::CleanUp()
{
    UnregisterLoadDelegates();

    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleObjectDestroyed);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleWindowResize);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleRaycastEvent);

    for (const auto& obj : gameObjects_)
        obj->Destroy();
    gameObjects_.clear();
    gameObjectIDMap_.clear();

    for (const auto& el : uiElements_)
        el->CleanUp();
    uiElements_.clear();
    uiElementIDMap_.clear();

    skybox_ = nullptr; root_ = nullptr; activeCamera_ = nullptr;
}

void ZScene::Finish()
{
    Stop();
    CleanUp();
    ZProcess::Finish();
}
