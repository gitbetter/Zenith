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
#include "ZGame.hpp"
#include "ZLight.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZGraphics.hpp"
#include "ZGameObject.hpp"
#include "ZSkybox.hpp"
#include "ZCameraComponent.hpp"
#include "ZPhysics.hpp"
#include "ZDomain.hpp"
#include "ZUI.hpp"
#include "ZUIText.hpp"
#include "ZUIElement.hpp"
#include "ZEventAgent.hpp"
#include "ZQuitEvent.hpp"
#include "ZSceneRoot.hpp"
#include "ZResourceHandle.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZGOFactory.hpp"
#include "ZUIFactory.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZSkyboxReadyEvent.hpp"
#include "ZSceneReadyEvent.hpp"

ZScene::ZScene(std::string name) : loadedResourceCount_(0), playState_(ZPlayState::NotStarted) {
    root_ = std::make_shared<ZSceneRoot>(name);
    root_->scene_ = this;
}

ZScene::ZScene(std::initializer_list<std::string> zofPaths) : ZScene() {
	// Make sure scene description paths are cached before loading them asynchronously.
    // This allows us to check if one of our zof files was loaded when receiving events
	for (std::string path : zofPaths) {
		pendingSceneDefinitions_[path] = true;
	}
}

void ZScene::Initialize() {
	ZEventDelegate quitDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleQuit);
	zenith::EventAgent()->AddListener(quitDelegate, ZQuitEvent::Type);

	ZEventDelegate zofLoadDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleZOFReady);
	zenith::EventAgent()->AddListener(zofLoadDelegate, ZResourceLoadedEvent::Type);

	ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleTextureReady);
	zenith::EventAgent()->AddListener(textureReadyDelegate, ZTextureReadyEvent::Type);

	ZEventDelegate shaderReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleShaderReady);
	zenith::EventAgent()->AddListener(shaderReadyDelegate, ZShaderReadyEvent::Type);

	ZEventDelegate modelReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleModelReady);
	zenith::EventAgent()->AddListener(modelReadyDelegate, ZModelReadyEvent::Type);

	ZEventDelegate skyboxReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleSkyboxReady);
	zenith::EventAgent()->AddListener(skyboxReadyDelegate, ZSkyboxReadyEvent::Type);

	for (auto it = pendingSceneDefinitions_.begin(); it != pendingSceneDefinitions_.end(); it++) {
		zenith::LoadZOF(it->first);
	}

    ZProcess::Initialize();
}

void ZScene::LoadSceneData(std::shared_ptr<ZOFTree> objectTree) {
	ParseSceneMetadata(objectTree);
    
	// TODO: The more systems are populated this way, the more of a hamper we place on
	// load times. Refactor this so the object tree is only traversed once.
	zenith::ScriptManager()->LoadAsync(objectTree);
	zenith::Graphics()->LoadAsync(objectTree);

	ZOFLoadResult zofResults;
	zofResults.gameObjects = zenith::GameObjectFactory()->Load(objectTree);
	zofResults.uiElements = zenith::UIFactory()->Load(objectTree);

	for (ZGameObjectMap::iterator it = zofResults.gameObjects.begin(); it != zofResults.gameObjects.end(); it++) {
		AddGameObject(it->second);
	}

	for (ZUIElementMap::iterator it = zofResults.uiElements.begin(); it != zofResults.uiElements.end(); it++) {
		AddUIElement(it->second);
	}
}

void ZScene::ParseSceneMetadata(std::shared_ptr<ZOFTree> objectTree) {
    bool hasSkybox = false;
    for (ZOFChildMap::iterator it = objectTree->children.begin(); it != objectTree->children.end(); it++) {
		if (it->first.find("ZSCENE") == 0) {
			std::shared_ptr<ZOFObjectNode> sceneDataNode = std::dynamic_pointer_cast<ZOFObjectNode>(it->second);
			if (sceneDataNode->properties.find("name") != sceneDataNode->properties.end()) {
				name_ = sceneDataNode->properties["name"]->Value<ZOFString>(0)->value;
                root_->SetName(name_);
			}
		} else if (it->first.find("ZTEX") == 0 || it->first.find("ZMOD") == 0 || it->first.find("ZSH") == 0 || 
				   /*it->first.find("ZSCR") == 0 ||*/ it->first.find("ZSKY") == 0) {
            pendingSceneObjects_.push_back(it->first);
            if (it->first.find("ZSKY") == 0)
                hasSkybox = true;
        }
    }

	loadedResourceCount_ = pendingSceneObjects_.size();
    
    if (!hasSkybox) SetDefaultSkybox();
}

void ZScene::Update() {
    if (zenith::Game()->Running()) {
        Render();
    }
}

void ZScene::Render() {
	UpdateViewProjectionMatrices();

	// TODO: Possible performance penalty here. Color and depth information might be better computed in 
	// a single render pass using multiple render targets.

	// Render pass #1: Shadow
	// TODO: Support more shadow casting lights!
	if (gameLights_.size() > 0) {
		zenith::Graphics()->SetupShadowDepthPass(gameLights_.begin()->second);
		root_->RenderChildren(ZRenderOp::Shadow);
		zenith::Graphics()->FinishRenderPass();
	}
    
    // Render pass #2: Depth
	zenith::Graphics()->SetupDepthPass();
    root_->RenderChildren(ZRenderOp::Depth);
	zenith::Graphics()->FinishRenderPass();
    
    // Render pass #3: Color
	zenith::Graphics()->SetupColorPass();
    root_->RenderChildren(ZRenderOp::Color);
	zenith::Graphics()->FinishRenderPass();

	// Render pass #4: Post-Processing
	zenith::Graphics()->SetupPostProcessingPass();
	zenith::Graphics()->PostProcessing(this);
	zenith::Graphics()->FinishRenderPass();
}

void ZScene::RenderUI() {
    for (ZUIElementMap::iterator it = uiElements_.begin(); it != uiElements_.end(); it++) {
        // Only render the top level elements that are not hidden. The children will
        // be rendered within the respective parent elements.
        if (!it->second->Hidden() && !it->second->Parent()) it->second->Render();
    }
}

void ZScene::Play() {
	playState_ = ZPlayState::Playing;
	zenith::Physics()->Resume();
}

void ZScene::Pause() {
	playState_ = ZPlayState::Paused;
	zenith::Physics()->Pause();
}

void ZScene::Stop() {
	playState_ = ZPlayState::NotStarted;
	zenith::Physics()->Pause();
}

void ZScene::UpdateViewProjectionMatrices() {
	previousViewProjection_ = viewProjection_;
	if (activeCamera_) {
		std::shared_ptr<ZCameraComponent> cameraComp = activeCamera_->FindComponent<ZCameraComponent>();
		glm::mat4 projectionMatrix = cameraComp->ProjectionMatrix();
		glm::mat4 viewMatrix = cameraComp->ViewMatrix();
		viewProjection_ = projectionMatrix * viewMatrix;
	} else {
		viewProjection_ = glm::mat4(1.f);
	}
}

void ZScene::AddGameObjects(std::initializer_list<std::shared_ptr<ZGameObject>> gameObjects) {
    for (std::shared_ptr<ZGameObject> object : gameObjects) {
        AddGameObject(object);
    }
}

void ZScene::AddGameObject(std::shared_ptr<ZGameObject> gameObject) {
    if (gameObject != nullptr) {
        gameObject->scene_ = this;
        if (gameObject->FindComponent<ZCameraComponent>() != nullptr) {
            activeCamera_ = primaryCamera_ = gameObject;
        }
        
        if (std::dynamic_pointer_cast<ZLight>(gameObject) != nullptr) {
            gameLights_.insert({gameObject->ID(), std::dynamic_pointer_cast<ZLight>(gameObject)});
        } else if (std::dynamic_pointer_cast<ZSkybox>(gameObject) != nullptr) {
            skybox_ = std::dynamic_pointer_cast<ZSkybox>(gameObject);
        } else {
            gameObjects_.insert({gameObject->ID(), gameObject});
        }
        root_->AddChild(gameObject);
    }
}

void ZScene::AddUIElement(std::shared_ptr<ZUIElement> element) {
    if (element != nullptr) {
        element->scene_ = this;
        if (std::dynamic_pointer_cast<ZUIText>(element)) element->SetShader(zenith::UI()->TextShader());
        else element->SetShader(zenith::UI()->UIShader());
        uiElements_[element->ID()] = element;
    }
}

void ZScene::AddUIElements(std::initializer_list<std::shared_ptr<ZUIElement>> elements) {
    for (std::shared_ptr<ZUIElement> element : elements) {
        AddUIElement(element);
    }
}

glm::mat4 ZScene::TopMatrix() {
    glm::mat4 M;
    sceneMutexes_.matrixStack.lock();
	if (matrixStack_.empty()) M = glm::mat4(1.f);
	else M = matrixStack_.back();
    sceneMutexes_.matrixStack.unlock();
    return M;
}

void ZScene::PushMatrix(glm::mat4 matrix) {
    sceneMutexes_.matrixStack.lock();
	matrixStack_.push_back(matrix);
    sceneMutexes_.matrixStack.unlock();
}

void ZScene::PopMatrix() {
    sceneMutexes_.matrixStack.lock();
	if (!matrixStack_.empty()) matrixStack_.pop_back();
    sceneMutexes_.matrixStack.unlock();
}

void ZScene::SetPrimaryCamera(std::shared_ptr<ZGameObject> gameObject) {
	if (gameObject && gameObject->FindComponent<ZCameraComponent>())
		primaryCamera_ = gameObject;
}

void ZScene::SetActiveCamera(std::shared_ptr<ZGameObject> gameObject) {
    if (gameObject && gameObject->FindComponent<ZCameraComponent>())
		activeCamera_ = gameObject;
}

void ZScene::SetDefaultSkybox() {
    skybox_ = std::shared_ptr<ZSkybox>(new ZSkybox(zenith::DEFAULT_HDR_CUBEMAP));
	skybox_->InitializeAsync();
	AddGameObject(skybox_);
}

void ZScene::UnregisterLoadDelegates() {
	ZEventDelegate zofLoadDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleZOFReady);
	zenith::EventAgent()->RemoveListener(zofLoadDelegate, ZResourceLoadedEvent::Type);

	ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleTextureReady);
	zenith::EventAgent()->RemoveListener(textureReadyDelegate, ZTextureReadyEvent::Type);

	ZEventDelegate shaderReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleShaderReady);
	zenith::EventAgent()->RemoveListener(shaderReadyDelegate, ZShaderReadyEvent::Type);

	ZEventDelegate modelReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleModelReady);
	zenith::EventAgent()->RemoveListener(modelReadyDelegate, ZModelReadyEvent::Type);

	ZEventDelegate skyboxReadyDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleSkyboxReady);
	zenith::EventAgent()->RemoveListener(skyboxReadyDelegate, ZSkyboxReadyEvent::Type);
}

void ZScene::HandleQuit(std::shared_ptr<ZEvent> event) {
    zenith::Domain()->Strategy()->ReleaseCursor();
}

void ZScene::HandleZOFReady(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZResourceLoadedEvent> loaded = std::dynamic_pointer_cast<ZResourceLoadedEvent>(event);
	if (!loaded->Handle()) return;

	if (pendingSceneDefinitions_.find(loaded->Handle()->Resource().name) != pendingSceneDefinitions_.end()) {
		std::shared_ptr<ZZOFResourceExtraData> extraData = std::dynamic_pointer_cast<ZZOFResourceExtraData>(loaded->Handle()->ExtraData());
		LoadSceneData(extraData->ObjectTree());
		pendingSceneDefinitions_.erase(loaded->Handle()->Resource().name);
	}

	if (pendingSceneDefinitions_.empty()) {
		ZEventDelegate zofLoadDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleZOFReady);
		zenith::EventAgent()->RemoveListener(zofLoadDelegate, ZResourceLoadedEvent::Type);
	}
}

void ZScene::HandleTextureReady(std::shared_ptr<ZEvent> event) {
	CheckPendingObject("ZTEX", event);
}

void ZScene::HandleShaderReady(std::shared_ptr<ZEvent> event) {
	CheckPendingObject("ZSH", event);
}

void ZScene::HandleSkyboxReady(std::shared_ptr<ZEvent> event) {
	CheckPendingObject("ZSKY", event);
}

void ZScene::HandleModelReady(std::shared_ptr<ZEvent> event) {
	CheckPendingObject("ZMOD", event);
}

void ZScene::CheckPendingObject(std::string type, std::shared_ptr<ZEvent>& event) {
	std::lock_guard<std::mutex> pendingObjectsLock(sceneMutexes_.pendingObjects);

	for (auto it = pendingSceneObjects_.begin(); it != pendingSceneObjects_.end(); it++) {
		if ((*it).find(type) != 0) continue;

		if (type == "ZTEX" && zenith::Graphics()->Textures().find(*it) != zenith::Graphics()->Textures().end()) {
			pendingSceneObjects_.erase(it);
			break;
		} else if (type == "ZSH" && zenith::Graphics()->Shaders().find(*it) != zenith::Graphics()->Shaders().end()) {
			pendingSceneObjects_.erase(it);
			break;
		} else if (type == "ZMOD" && zenith::Graphics()->Models().find(*it) != zenith::Graphics()->Models().end()) {
			pendingSceneObjects_.erase(it);
			break;
		} else if (type == "ZSCR" && zenith::ScriptManager()->Scripts().find(*it) != zenith::ScriptManager()->Scripts().end()) {
			pendingSceneObjects_.erase(it);
			break;
		} else if (type == "ZSKY") {
			std::shared_ptr<ZSkyboxReadyEvent> ready = std::dynamic_pointer_cast<ZSkyboxReadyEvent>(event);
			if (ready->Skybox() == skybox_) {
				pendingSceneObjects_.erase(it);
				break;
			}
		}
	}

	if (pendingSceneObjects_.empty()) {
		UnregisterLoadDelegates();
		std::shared_ptr<ZSceneReadyEvent> sceneReadyEvent = std::make_shared<ZSceneReadyEvent>(shared_from_this());
		zenith::EventAgent()->QueueEvent(sceneReadyEvent);
	}
}

void ZScene::CleanUp() {
    ZEventDelegate quitDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleQuit);
    zenith::EventAgent()->RemoveListener(quitDelegate, ZQuitEvent::Type);
    
    UnregisterLoadDelegates();
    
    for (ZGameObjectMap::iterator it = gameObjects_.begin(); it != gameObjects_.end(); it++) {
        it->second->CleanUp();
    }
    gameObjects_.clear();
    
    for (ZUIElementMap::iterator it = uiElements_.begin(); it != uiElements_.end(); it++) {
        it->second->CleanUp();
    }
    uiElements_.clear();
    
    skybox_ = nullptr; root_ = nullptr; activeCamera_ = nullptr;
}
