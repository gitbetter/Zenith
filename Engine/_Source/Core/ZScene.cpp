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
        ZServices::GameObjectManager()->Prepare(root_, deltaTime);
        ZServices::UIElementManager()->Prepare(canvas_, deltaTime);
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
    if (!activeCamera_.IsNull()) {
        ZCamera* camera = ZServices::GameObjectManager()->Dereference<ZCamera>(activeCamera_);
        camera->EnableLook();
        camera->EnableMovement();
    }
}

void ZScene::Pause()
{
    playState_ = ZPlayState::Paused;
    gameSystems_.physics->Pause();
    if (!activeCamera_.IsNull()) {
        ZCamera* camera = ZServices::GameObjectManager()->Dereference<ZCamera>(activeCamera_);
        camera->DisableLook();
        camera->DisableMovement();
    }
}

void ZScene::Stop()
{
    playState_ = ZPlayState::Ready;
    gameSystems_.physics->Pause();
    if (!activeCamera_.IsNull()) {
        ZCamera* camera = ZServices::GameObjectManager()->Dereference<ZCamera>(activeCamera_);
        camera->DisableLook();
        camera->DisableMovement();
    }
}

void ZScene::UpdateLightspaceMatrices()
{
    ZCamera* camera = ZServices::GameObjectManager()->Dereference<ZCamera>(activeCamera_);
    if (gameLights_.empty() || !camera->isMoving)
    {
        return;
    }

    for (const auto& light : gameLights_)
    {
        ZLight* lightObject = ZServices::GameObjectManager()->Dereference<ZLight>(light);
        lightObject->UpdateLightspaceMatrices(camera->frustum);
    }
}

ZSceneSnapshot ZScene::Snapshot()
{
    ZSceneSnapshot snapshot;
    std::shared_ptr<ZScene> sceneClone = std::make_shared<ZScene>(name_);

    sceneClone->playState_ = playState_;
    sceneClone->state_ = state_;

    sceneClone->AddGameObject(ZServices::GameObjectManager()->Clone(skybox_), false);
    for (const auto& obj : gameObjects_)
    {
        sceneClone->AddGameObject(ZServices::GameObjectManager()->Clone(obj), false);
    }

    for (const auto& el : uiElements_)
    {
        sceneClone->AddUIElement(el); // TODO: Clone UI Elements too
    }

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
    {
        ZServices::GameObjectManager()->Destroy(*gameObjects_.begin());
    }

    gameObjects_.clear();
    uiElements_.clear();
    gameLights_.clear();
    gameObjectIDMap_.clear();
    uiElementIDMap_.clear();
    gameLightIDMap_.clear();

    AddGameObject(snapshot.scene->skybox_);

    for (const auto& obj : snapshot.scene->gameObjects_)
    {
        AddGameObject(obj);
    }

    for (const auto& el : snapshot.scene->uiElements_)
    {
        AddUIElement(el);
    }
}

void ZScene::AddBVHPrimitive(const ZBVHPrimitive& primitive)
{
    bvh_->AddPrimitive(primitive);
}

void ZScene::CreateSceneRoot(const std::string& name)
{
    root_ = ZServices::GameObjectManager()->CreateReady(ZGameObjectType::SceneRoot);
    ZServices::GameObjectManager()->SetName(root_, name);
    ZServices::GameObjectManager()->SetScene(root_, shared_from_this());
}

void ZScene::CreateUICanvas()
{
    ZUIElementOptions elementOptions;
    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    elementOptions.color = glm::vec4(1.f);
    canvas_ = ZServices::UIElementManager()->Create(ZUIElementType::Canvas, elementOptions, ZHUIElement(), shared_from_this());
}

void ZScene::AddGameObjects(std::initializer_list<ZHGameObject> gameObjects, bool runImmediately)
{
    for (const ZHGameObject& object : gameObjects)
    {
        AddGameObject(object, runImmediately);
    }
}

void ZScene::AddGameObject(const ZHGameObject& gameObject, bool runImmediately)
{
    if (!gameObject.IsNull())
    {
        ZServices::GameObjectManager()->SetScene(gameObject, shared_from_this());
        std::string objectName = ZServices::GameObjectManager()->Name(gameObject);
        if (ZCamera* camera = ZServices::GameObjectManager()->Dereference<ZCamera>(gameObject))
        {
            if (camera->isPrimary)
            {
                primaryCamera_ = gameObject;
            }
            activeCamera_ = gameObject;
        } 
        else if (ZLight* light = ZServices::GameObjectManager()->Dereference<ZLight>(gameObject))
        {
            if (gameLightIDMap_.find(objectName) == gameLightIDMap_.end()) {
                gameLightIDMap_[objectName] = gameLights_.size();
                gameLights_.push_back(gameObject);
            }
        }

        if (ZSkybox* skybox = ZServices::GameObjectManager()->Dereference<ZSkybox>(gameObject))
        {
            skybox_ = gameObject;
        }
        else
        {
            if (gameObjectIDMap_.find(objectName) == gameObjectIDMap_.end()) {
                gameObjectIDMap_[objectName] = gameObjects_.size();
                gameObjects_.push_back(gameObject);
            }
        }

        if (!ZServices::GameObjectManager()->Parent(gameObject))
        {
            ZServices::GameObjectManager()->AddChild(root_, gameObject);
        }

        if (runImmediately)
        {
            for (auto comp : ZServices::GameObjectManager()->Components(gameObject))
            {
                ZServices::ProcessRunner(gameName_)->AttachProcess(comp);
            }
        }

        for (const ZHGameObject& child : ZServices::GameObjectManager()->Children(gameObject))
        {
            AddGameObject(child);
        }
    }
}

void ZScene::RemoveGameObject(const ZHGameObject& gameObject)
{
    if (!gameObject.IsNull())
    {
        if (primaryCamera_ == gameObject)
        {
            primaryCamera_ = ZHGameObject();
        }
        else if (activeCamera_ == gameObject)
        {
            activeCamera_ = ZHGameObject();
        }

        std::string objectName = ZServices::GameObjectManager()->Name(gameObject);

        if (gameLightIDMap_.find(objectName) != gameLightIDMap_.end())
        {
            gameLights_.erase(gameLights_.begin() + gameLightIDMap_[objectName]);
            gameLightIDMap_.erase(objectName);
        }
        if (gameObjectIDMap_.find(objectName) != gameObjectIDMap_.end())
        {
            gameObjects_.erase(gameObjects_.begin() + gameObjectIDMap_[objectName]);
            gameObjectIDMap_.erase(objectName);
        }

        if (const ZHGameObject& parent = ZServices::GameObjectManager()->Parent(gameObject))
        {
            if (parent == root_)
            {
                ZServices::GameObjectManager()->RemoveChild(root_, gameObject, true);
            }
        }
        ZServices::GameObjectManager()->SetParent(gameObject, ZHGameObject());
        ZServices::GameObjectManager()->SetScene(gameObject, nullptr);

        for (const ZHGameObject& child : ZServices::GameObjectManager()->Children(gameObject))
        {
            RemoveGameObject(child);
        }
    }
}

ZHGameObject ZScene::FindGameObjectByName(const std::string& name)
{
    if (!name.empty())
    {
        for (const auto& obj : gameObjects_)
        {
            if (zenith::strings::HasSuffix(ZServices::GameObjectManager()->Name(obj), name))
            {
                return obj;
            }
            else if (ZServices::GameObjectManager()->HasChildren(obj))
            {
                const ZHGameObject& found = ZServices::GameObjectManager()->Child<ZGameObject>(obj, name);
                if (!found.IsNull())
                {
                    return found;
                }
            }
        }
    }
    return ZHGameObject();
}

void ZScene::AddUIElement(const ZHUIElement& element)
{
    const std::string elementName = ZServices::UIElementManager()->Name(element);
    if (element && uiElementIDMap_.find(elementName) == uiElementIDMap_.end())
    {
        ZServices::UIElementManager()->AddChild(canvas_, element);
        uiElementIDMap_[elementName] = uiElements_.size();
        uiElements_.push_back(element);
    }
}

void ZScene::AddUIElements(std::initializer_list<ZHUIElement> elements)
{
    for (const ZHUIElement& element : elements)
    {
        AddUIElement(element);
    }
}

void ZScene::RemoveUIElement(const ZHUIElement& element)
{
    auto elementName = ZServices::UIElementManager()->Name(element);
    if (element && uiElementIDMap_.find(elementName) != uiElementIDMap_.end())
    {
        uiElements_.erase(uiElements_.begin() + uiElementIDMap_[elementName]);
        uiElementIDMap_.erase(elementName);
        ZServices::UIElementManager()->RemoveChild(canvas_, element, true);
        ZServices::UIElementManager()->SetParent(element, ZHUIElement());
        ZServices::UIElementManager()->SetScene(element, nullptr);
    }
}

ZHUIElement ZScene::FindUIElement(const std::string& name)
{
    if (!name.empty())
    {
        for (const ZHUIElement& el : uiElements_)
        {
            const std::string elementName = ZServices::UIElementManager()->Name(el);
            if (zenith::strings::HasSuffix(elementName, name))
            {
                return el;
            }
            else if (ZServices::UIElementManager()->HasChildren(el))
            {
                const ZHUIElement& found = ZServices::UIElementManager()->ChildByName(el, name);
                if (!found.IsNull())
                {
                    return found;
                }
            }
        }
    }
    return ZHUIElement();
}

ZRay ZScene::ScreenPointToWorldRay(const glm::vec2& point, const glm::vec2& dimensions)
{
    auto cam = ActiveCamera();
    if (cam.IsNull())
    {
        return ZRay(glm::vec3(0.f), glm::vec3(0.f));
    }

    ZCamera* camObject = ZServices::GameObjectManager()->Dereference<ZCamera>(cam);

    auto rectRes = dimensions == glm::vec2(0.f) ? gameSystems_.domain->Resolution() : dimensions;
    float x = (2.f * point.x) / rectRes.x - 1.f;
    float y = (2.f * point.y) / rectRes.y - 1.f;

    glm::vec4 start = glm::normalize(camObject->inverseViewProjection * glm::vec4(x, -y, 0.f, 1.f));
    start /= start.w;
    glm::vec4 end = glm::normalize(camObject->inverseViewProjection * glm::vec4(x, -y, 1.f, 1.f));
    end /= end.w;
    glm::vec4 direction = glm::normalize(end - start);

    return ZRay(ZServices::GameObjectManager()->Position(cam), direction);
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
    skybox_ = ZServices::GameObjectManager()->CreateReady(ZGameObjectType::Skybox);
    ZSkybox* skyboxObject = ZServices::GameObjectManager()->Dereference<ZSkybox>(skybox_);
    skyboxObject->LoadCubemap(DEFAULT_HDR_CUBEMAP);
    AddGameObject(skybox_);
}

void ZScene::LoadSceneData(const std::shared_ptr<ZOFNode>& objectTree)
{
    ParseSceneMetadata(objectTree);

	for (ZOFChildList::iterator it = objectTree->children.begin(); it != objectTree->children.end(); it++)
	{
		std::shared_ptr<ZOFObjectNode> dataNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
        ZHGameObject gameObject;
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
            case ZOFObjectType::Light:
            case ZOFObjectType::Camera:
            case ZOFObjectType::Skybox:
            case ZOFObjectType::Grass:
            {
                gameObject = ZServices::GameObjectManager()->Deserialize(dataNode->id, dataNode, shared_from_this());
                break;
            }
            case ZOFObjectType::UI:
            {
                ZHUIElement element = ZServices::UIElementManager()->Deserialize(dataNode->id, dataNode, shared_from_this());
                AddUIElement(element);
            }
            default: break;
        }

		if (!gameObject.IsNull())
        {
			for (ZOFChildList::iterator compIt = dataNode->children.begin(); compIt != dataNode->children.end(); compIt++)
			{
				std::shared_ptr<ZOFObjectNode> componentNode = std::static_pointer_cast<ZOFObjectNode>(compIt->second);
				ZComponent::CreateIn(gameObject, componentNode);
			}
            AddGameObject(gameObject);
		}
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
                ZServices::GameObjectManager()->SetName(root_, name_);
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
        std::shared_ptr<ZObjectSelectedEvent> objectSelectEvent = std::make_shared<ZObjectSelectedEvent>(hitResult.objectHit, hitResult.hitPosition);
        ZServices::EventAgent()->Trigger(objectSelectEvent);
    }
}

void ZScene::CleanUp()
{
    UnregisterLoadDelegates();

    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleObjectDestroyed);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleWindowResize);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleRaycastEvent);

    for (const ZHGameObject& obj : gameObjects_)
    {
        ZServices::GameObjectManager()->Destroy(obj);
    }

    gameObjects_.clear();
    gameObjectIDMap_.clear();
    uiElements_.clear();
    uiElementIDMap_.clear();
}

void ZScene::Finish()
{
    Stop();
    CleanUp();
    ZProcess::Finish();
}
