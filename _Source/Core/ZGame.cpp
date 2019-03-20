/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGame.cpp

    Created by Adrian Sanchez on 1/27/19.
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

#include "ZGame.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZDomainStrategy.hpp"
#include "ZInput.hpp"
#include "ZUI.hpp"
#include "ZPhysics.hpp"
#include "ZModel.hpp"
#include "ZSkybox.hpp"
#include "ZShader.hpp"
#include "ZCameraComponent.hpp"
#include "ZEventAgent.hpp"
#include "ZQuitEvent.hpp"

#include <chrono>
using namespace std;

ZGame::ZGame() : activeCameraObject_("") { }

void ZGame::Setup() {
  ZEventDelegate quitDelegate = fastdelegate::MakeDelegate(this, &ZGame::HandleQuit);
  ZEngine::EventAgent()->AddListener(quitDelegate, ZQuitEvent::Type);
}

void ZGame::RunGameLoop() {
  Setup();

  _Z("Zenith is about to loop...", ZINFO);

  float previousTime = ZEngine::SecondsTime();
  float lag = 0.0;

  while (!ZEngine::Domain()->Strategy()->IsWindowClosing()) {
    int fixedUpdates = 0;
    float currentTime = ZEngine::SecondsTime();
    ZEngine::SetDeltaTime(currentTime - previousTime);
    previousTime = currentTime;
    lag += ZEngine::DeltaTime();

    ZEngine::EventAgent()->Process(ZEngine::UPDATE_STEP_SIZE * 2.f);

    ZEngine::Input()->Process();

    while (lag >= ZEngine::UPDATE_STEP_SIZE && ++fixedUpdates <= ZEngine::MAX_FIXED_UPDATE_ITERATIONS) {
      Update();
      lag -= ZEngine::UPDATE_STEP_SIZE;
    }

    Render(lag / ZEngine::UPDATE_STEP_SIZE);
    ZEngine::Domain()->Strategy()->PollEvents();

    MacDisplayHack();
  }
}

void ZGame::Update() {
  ZEngine::Physics()->Update();
  for (auto it = gameObjects_.begin(); it != gameObjects_.end(); it++) {
    it->second->Update();
  }
}

void ZGame::Render(float frameMix, RENDER_OP renderOp) {
  ZEngine::Graphics()->Draw(gameObjects_, gameLights_, frameMix);
  if (skybox_ != nullptr) skybox_->Render(frameMix, renderOp);

  // TODO: If the UI has changed, draw it. Otherwise, leave it.
  // The dirty flag trick might come in handy here
  ZEngine::UI()->Draw();

  //ZEngine::Physics()->DebugDraw();

  ZEngine::Graphics()->Strategy()->SwapBuffers();
}

void ZGame::AddGameObject(std::shared_ptr<ZGameObject> gameObject) {
  if (gameObject != nullptr) {
    gameObject->game_ = this;
    if (gameObject->FindComponent<ZCameraComponent>() != nullptr) {
      activeCameraObject_ = gameObject->ID();
    }

    if (dynamic_pointer_cast<ZLight>(gameObject) != nullptr) {
      gameLights_.insert({gameObject->ID(), dynamic_pointer_cast<ZLight>(gameObject)});
    } else {
      gameObjects_.insert({gameObject->ID(), gameObject});
    }
  }
}

void ZGame::AddGameObjects(std::initializer_list<std::shared_ptr<ZGameObject>> gameObjects) {
  for (std::shared_ptr<ZGameObject> object : gameObjects) {
    AddGameObject(object);
  }
}

void ZGame::SetDefaultSkybox() {
  std::shared_ptr<ZSkybox> skybox(new ZSkybox);
  skybox->Initialize(ZEngine::DEFAULT_HDR_CUBEMAP);
  skybox->game_ = this;

  skybox_ = skybox;
}

std::shared_ptr<ZGameObject> ZGame::ActiveCamera() {
  if (gameObjects_.find(activeCameraObject_) == gameObjects_.end()) return nullptr;
  return gameObjects_[activeCameraObject_];
}

void ZGame::HandleQuit(std::shared_ptr<ZEvent> event) {
  ZEngine::Domain()->Strategy()->ReleaseCursor();
}

// -.-
void ZGame::MacDisplayHack() {
  #ifdef __APPLE__
  static bool moved = false;
  if (!moved) {
    ZEngine::Domain()->ResizeWindow(
      ZEngine::Domain()->WindowWidth() - 1,
      ZEngine::Domain()->WindowHeight() - 1
    );
    moved = true;
  }
  #endif
}
