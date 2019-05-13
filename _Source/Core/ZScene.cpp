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
#include "ZLight.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGameObject.hpp"
#include "ZSkybox.hpp"
#include "ZCameraComponent.hpp"
#include "ZDomain.hpp"
#include "ZPhysics.hpp"
#include "ZUI.hpp"
#include "ZEventAgent.hpp"
#include "ZQuitEvent.hpp"
#include "ZSceneRoot.hpp"

ZScene::ZScene() {
    root_ = std::make_shared<ZSceneRoot>();
    root_->scene_ = this;
}

void ZScene::Initialize() {
    ZEventDelegate quitDelegate = fastdelegate::MakeDelegate(this, &ZScene::HandleQuit);
    ZEngine::EventAgent()->AddListener(quitDelegate, ZQuitEvent::Type);
    ZProcess::Initialize();
}

void ZScene::Update() {
    if (!ZEngine::Domain()->Strategy()->IsWindowClosing()) {
        Render();
    }
}

void ZScene::Render() {
    ZEngine::Graphics()->Strategy()->ClearViewport();
    
    float frameMix = glm::clamp((float)ZEngine::DeltaTime() - (ZEngine::UPDATE_STEP_SIZE * (float)ZEngine::MAX_FIXED_UPDATE_ITERATIONS),
                                0.f, 1.f);
    
    // TODO: Support more shadow casting lights!
    if (gameLights_.size() > 0) {
        ZEngine::Graphics()->SetupShadowPass(gameLights_.begin()->second);
        root_->RenderChildren(frameMix, RENDER_OP_SHADOW);
        ZEngine::Graphics()->FinishShadowPass();
    }
    
    root_->RenderChildren(frameMix);
    
    ZEngine::UI()->Draw();
    
    ZEngine::Physics()->DebugDraw();
    
    ZEngine::Graphics()->Strategy()->SwapBuffers();
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
            activeCamera_ = gameObject;
        }
        
        if (std::dynamic_pointer_cast<ZLight>(gameObject) != nullptr) {
            gameLights_.insert({gameObject->ID(), std::dynamic_pointer_cast<ZLight>(gameObject)});
        } else {
            gameObjects_.insert({gameObject->ID(), gameObject});
        }
        root_->AddChild(gameObject);
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

void ZScene::SetActiveCamera(std::shared_ptr<ZGameObject> gameObject) {
    if (gameObject && gameObject->FindComponent<ZCameraComponent>())
    activeCamera_ = gameObject;
}

void ZScene::SetDefaultSkybox() {
    skybox_ = std::shared_ptr<ZSkybox>(new ZSkybox);
	skybox_->Initialize(ZEngine::DEFAULT_HDR_CUBEMAP);
	AddGameObject(skybox_);
}

void ZScene::HandleQuit(std::shared_ptr<ZEvent> event) {
    ZEngine::Domain()->Strategy()->ReleaseCursor();
}
