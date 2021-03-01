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
#include "ZAssetStore.hpp"
#include "ZRenderer.hpp"
#include "ZFramebuffer.hpp"
#include "ZGame.hpp"
#include "ZLight.hpp"
#include "ZSkybox.hpp"
#include "ZCamera.hpp"
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
#include "ZResourceExtraData.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZSkyboxReadyEvent.hpp"
#include "ZSceneReadyEvent.hpp"
#include "ZObjectDestroyedEvent.hpp"
#include "ZRaycastEvent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZWindowResizeEvent.hpp"
#include "ZStringHelpers.hpp"

ZScene::ZScene(const std::string& name) : name_(name), playState_(ZPlayState::Loading)
{
}

ZScene::ZScene(std::initializer_list<std::string> zofPaths) : ZScene()
{
// Make sure scene description paths are cached before loading them asynchronously.
// This allows us to check if one of our zof files was loaded when receiving events
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
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleTextureReady);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleShaderReady);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleModelReady);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleSkyboxReady);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleObjectDestroyed);
    ZServices::EventAgent()->Subscribe(this, &ZScene::HandleRaycastEvent);

    CreateSceneRoot(name_);
    CreateUICanvas();

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
    if (playState_ == ZPlayState::Playing || playState_ == ZPlayState::Paused) {
        root_->Prepare(deltaTime);
        canvas_->Prepare(deltaTime);
        renderer_->Render(deltaTime);
    }
    else if (playState_ == ZPlayState::Loading) {
        CheckPendingObjects();
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
    for (auto it = gameLights_.begin(); it != gameLights_.end(); it++) {
        it->second->UpdateLightspaceMatrices(frustum);
    }
}

ZSceneSnapshot ZScene::Snapshot()
{
    ZSceneSnapshot snapshot;
    std::shared_ptr<ZScene> sceneClone = std::make_shared<ZScene>(name_);

    sceneClone->playState_ = playState_;
    sceneClone->state_ = state_;

    sceneClone->AddGameObject(skybox_->Clone(), false);
    for (auto pair : gameObjects_)
        sceneClone->AddGameObject(pair.second->Clone(), false);

    for (auto pair : uiElements_)
        sceneClone->AddUIElement(pair.second); // TODO: Clone UI Elements too

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
        gameObjects_.begin()->second->Destroy();

    gameObjects_.clear(); uiElements_.clear(); gameLights_.clear();

    AddGameObject(snapshot.scene->skybox_);
    for (auto pair : snapshot.scene->gameObjects_)
        AddGameObject(pair.second);

    for (auto pair : snapshot.scene->uiElements_)
        AddUIElement(pair.second);
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
            gameLights_.insert({ gameObject->ID(), light });
        }

        if (auto skybox = std::dynamic_pointer_cast<ZSkybox>(gameObject))
        {
            skybox_ = skybox;
        }
        else
        {
            gameObjects_.insert({ gameObject->ID(), gameObject });
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
        else if (gameLights_.find(gameObject->ID()) != gameLights_.end()) 
        {
            gameLights_.erase(gameObject->ID());
        }

        gameObjects_.erase(gameObject->ID());
        if (auto parent = gameObject->Parent()) {
            if (parent == root_)
                root_->RemoveChild(gameObject, true);
        }
        gameObject->parent_.reset();
        gameObject->SetScene(nullptr);

        for (auto child : gameObject->Children()) {
            RemoveGameObject(child);
        }
    }
}

std::shared_ptr<ZGameObject> ZScene::FindGameObject(const std::string& id)
{
    if (!id.empty())
    {
        for (auto it = gameObjects_.begin(); it != gameObjects_.end(); it++)
        {
            if (zenith::strings::HasSuffix(it->first, id))
            {
                return it->second;
            }
            else if (it->second->HasChildren())
            {
                auto found = it->second->Child<ZGameObject>(id);
                if (found) return found;
            }
        }
    }
    return nullptr;
}

void ZScene::AddUIElement(std::shared_ptr<ZUIElement> element)
{
    if (element != nullptr)
    {
        canvas_->AddChild(element);
        uiElements_[element->ID()] = element;
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
    if (element != nullptr)
    {
        uiElements_.erase(element->ID());
        canvas_->RemoveChild(element, true);
        element->parent_.reset();
        element->scene_.reset();
    }
}

std::shared_ptr<ZUIElement> ZScene::FindUIElement(const std::string& id)
{
    if (!id.empty())
    {
        for (auto it = uiElements_.begin(); it != uiElements_.end(); it++)
        {
            if (zenith::strings::HasSuffix(it->first, id))
            {
                return it->second;
            }
            else if (it->second->HasChildren())
            {
                auto found = it->second->Child<ZUIElement>(id);
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

    return ZRay(start, direction);
}

std::shared_ptr<ZTexture> ZScene::TargetTexture()
{
    if (targetBuffer_)
        return targetBuffer_->BoundAttachment();
    return nullptr;
}

void ZScene::SetDefaultSkybox()
{
    skybox_ = std::shared_ptr<ZSkybox>(new ZSkybox(DEFAULT_HDR_CUBEMAP));
    skybox_->InitializeAsync();
    AddGameObject(skybox_);
}

void ZScene::LoadSceneData(const std::shared_ptr<ZOFTree>& objectTree)
{
    ParseSceneMetadata(objectTree);

    // TODO: The more systems are populated this way, the more of a hamper we place on
    // load times. Refactor this so the object tree is only traversed once, or erase
    // ZOF iterators as items are created.
    ZServices::ScriptManager()->LoadAsync(objectTree);
    ZServices::AssetStore()->LoadAsync(objectTree);

    ZOFLoadResult zofResults;
    zofResults.gameObjects = ZGameObject::Load(objectTree, shared_from_this());
    zofResults.uiElements = ZUIElement::Load(objectTree, shared_from_this());

    for (ZGameObjectMap::iterator it = zofResults.gameObjects.begin(); it != zofResults.gameObjects.end(); it++)
    {
        AddGameObject(it->second);
    }

    for (ZUIElementMap::iterator it = zofResults.uiElements.begin(); it != zofResults.uiElements.end(); it++)
    {
        AddUIElement(it->second);
    }
}

void ZScene::ParseSceneMetadata(const std::shared_ptr<ZOFTree>& objectTree)
{
    bool hasSkybox = false;
    for (ZOFChildMap::iterator it = objectTree->children.begin(); it != objectTree->children.end(); it++)
    {
        if (it->first.find("ZSCENE") == 0)
        {
            std::shared_ptr<ZOFObjectNode> sceneDataNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (sceneDataNode->properties.find("name") != sceneDataNode->properties.end())
            {
                name_ = sceneDataNode->properties["name"]->Value<ZOFString>(0)->value;
                root_->SetName(name_);
            }
        }
        else if (it->first.find("ZTEX") == 0 || it->first.find("ZMOD") == 0 || it->first.find("ZSH") == 0 ||
            /*it->first.find("ZSCR") == 0 ||*/ it->first.find("ZSKY") == 0)
        {
            std::lock_guard<std::mutex> pendingObjectsLock(sceneMutexes_.pendingObjects);
            pendingSceneObjects_.push_back(it->first);
            if (it->first.find("ZSKY") == 0)
                hasSkybox = true;
        }
    }

    if (!hasSkybox && gameConfig_.graphics.clearColor == glm::vec4(0.f, 0.f, 0.f, 1.f))
        SetDefaultSkybox();
}

void ZScene::UnregisterLoadDelegates()
{
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleZOFReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleTextureReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleShaderReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleModelReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleSkyboxReady);
}

void ZScene::CheckPendingObjects()
{
    std::lock_guard<std::mutex> pendingObjectsLock(sceneMutexes_.pendingObjects);
    if (pendingSceneObjects_.empty())
    {
        UnregisterLoadDelegates();
        playState_ = ZPlayState::Ready;
        std::shared_ptr<ZSceneReadyEvent> sceneReadyEvent = std::make_shared<ZSceneReadyEvent>(shared_from_this());
        ZServices::EventAgent()->Queue(sceneReadyEvent);
    }
}

void ZScene::HandleWindowResize(const std::shared_ptr<ZWindowResizeEvent>& event)
{
    glm::vec2 newResolution = gameSystems_.domain->Resolution();
    if (targetBuffer_) {
        targetBuffer_->Resize(newResolution.x, newResolution.y);
    }
    for (auto pass : renderer_->Passes()) {
        pass->SetSize(newResolution);
    }
}

void ZScene::HandleZOFReady(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (!event->Handle()) return;

    if (pendingSceneDefinitions_.find(event->Handle()->Resource().name) != pendingSceneDefinitions_.end())
    {
        std::shared_ptr<ZZOFResourceExtraData> extraData = std::static_pointer_cast<ZZOFResourceExtraData>(event->Handle()->ExtraData());
        LoadSceneData(extraData->ObjectTree());
        pendingSceneDefinitions_.erase(event->Handle()->Resource().name);
        playState_ = ZPlayState::Loading;
    }

    if (pendingSceneDefinitions_.empty())
    {
        ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleZOFReady);
    }
}

void ZScene::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    auto texture = event->Texture();
    std::lock_guard<std::mutex> pendingObjectsLock(sceneMutexes_.pendingObjects);
    auto it = std::find(pendingSceneObjects_.begin(), pendingSceneObjects_.end(), texture->name);
    if (it != pendingSceneObjects_.end())
    {
        pendingSceneObjects_.erase(it);
    }
}

void ZScene::HandleShaderReady(const std::shared_ptr<ZShaderReadyEvent>& event)
{
    auto shader = event->Shader();
    std::lock_guard<std::mutex> pendingObjectsLock(sceneMutexes_.pendingObjects);
    auto it = std::find(pendingSceneObjects_.begin(), pendingSceneObjects_.end(), shader->Name());
    if (it != pendingSceneObjects_.end())
    {
        pendingSceneObjects_.erase(it);
    }
}

void ZScene::HandleSkyboxReady(const std::shared_ptr<ZSkyboxReadyEvent>& event)
{
    auto skybox = event->Skybox();
    std::lock_guard<std::mutex> pendingObjectsLock(sceneMutexes_.pendingObjects);
    auto it = std::find(pendingSceneObjects_.begin(), pendingSceneObjects_.end(), skybox->ID());
    if (it != pendingSceneObjects_.end())
    {
        pendingSceneObjects_.erase(it);
    }
}

void ZScene::HandleModelReady(const std::shared_ptr<ZModelReadyEvent>& event)
{
    auto model = event->Model();
    std::lock_guard<std::mutex> pendingObjectsLock(sceneMutexes_.pendingObjects);
    auto it = std::find(pendingSceneObjects_.begin(), pendingSceneObjects_.end(), model->ID());
    if (it != pendingSceneObjects_.end())
    {
        pendingSceneObjects_.erase(it);
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
        std::shared_ptr<ZObjectSelectedEvent> objectSelectEvent(new ZObjectSelectedEvent(hitResult.objectHit->ID(), hitResult.hitPosition));
        ZServices::EventAgent()->Trigger(objectSelectEvent);
    }
}

void ZScene::CleanUp()
{
    UnregisterLoadDelegates();

    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleObjectDestroyed);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleWindowResize);
    ZServices::EventAgent()->Unsubscribe(this, &ZScene::HandleRaycastEvent);

    for (ZGameObjectMap::iterator it = gameObjects_.begin(); it != gameObjects_.end(); it++)
        it->second->Destroy();
    gameObjects_.clear();

    for (ZUIElementMap::iterator it = uiElements_.begin(); it != uiElements_.end(); it++)
        it->second->CleanUp();
    uiElements_.clear();

    skybox_ = nullptr; root_ = nullptr; activeCamera_ = nullptr;
}

void ZScene::Finish()
{
    Stop();
    CleanUp();
    ZProcess::Finish();
}
