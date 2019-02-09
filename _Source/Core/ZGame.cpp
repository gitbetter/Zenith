//
//  ZGame.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/27/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGame.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZDomainStrategy.hpp"
#include "ZInput.hpp"
#include "ZUI.hpp"
#include "ZCamera.hpp"
#include "ZLogger.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZActor.hpp"
#include "ZLogger.hpp"

#include <chrono>
#include <cassert>
using namespace std;

ZGame::ZGame() : activeCameraIndex_(-1) { }

void ZGame::RunGameLoop() {
  ZLogger::Log("Zenith is about to loop...", ZLoggerSeverity::Info);

  float previousTime = ZEngine::MilliSecondTime();
  float lag = 0.0;

  while (!ZEngine::Domain()->Strategy()->IsWindowClosing()) {
    int fixedUpdates = 0;
    float currentTime = ZEngine::MilliSecondTime();
    ZEngine::SetDeltaTime(currentTime - previousTime);
    previousTime = currentTime;
    lag += ZEngine::DeltaTime();

    ZEngine::Input()->Process();
    MacDisplayHack();

    while (lag >= ZEngine::MS_PER_UPDATE && ++fixedUpdates <= ZEngine::MAX_FIXED_UPDATE_ITERATIONS) {
        Update();
        lag -= ZEngine::MS_PER_UPDATE;
    }

    Render(lag / ZEngine::MS_PER_UPDATE);
    ZEngine::Domain()->Strategy()->PollEvents();
  }
}

void ZGame::Update() {
  for (unsigned int i = 0; i < gameObjects_.size(); i++) {
      gameObjects_[i]->Update();
  }
}

void ZGame::Render(float frameMix, unsigned char renderOp) {
  ZEngine::Graphics()->Draw(gameObjects_, gameLights_, frameMix);
  // TODO: If the UI has changed, draw it. Otherwise, leave it.
  // The dirty flag trick might come in handy here
  ZEngine::UI()->Draw();
  
  ZEngine::Graphics()->Strategy()->SwapBuffers();
}

void ZGame::AddGameObject(ZGameObject* gameObject) {
  if (gameObject != nullptr) {
    gameObject->game_ = this;
    if (dynamic_cast<ZCamera*>(gameObject)) {
      gameCameras_.push_back(dynamic_cast<ZCamera*>(gameObject));
      activeCameraIndex_ += 1;
    } else if (dynamic_cast<ZLight*>(gameObject)) {
      gameLights_.push_back(dynamic_cast<ZLight*>(gameObject));
    } else {
      gameObjects_.push_back(gameObject);
    }
  }
}

void ZGame::AddGameObjects(std::initializer_list<ZGameObject*> gameObjects) {
  for (ZGameObject* object : gameObjects) {
    AddGameObject(object);
  }
}

void ZGame::SetDefaultSkybox() {
  ZModel* skybox = ZModel::NewSkybox();
  // ... and a special set of skybox shaders.
  ZShader* skyboxShader = new ZShader("Assets/Shaders/Vertex/skybox.vert", "Assets/Shaders/Pixel/skybox.frag");
  ZGraphicsComponent* skyboxGraphicsComponent = new ZGraphicsComponent(skybox, skyboxShader);

  ZActor* skyboxActor = new ZActor();
  skyboxActor->AddComponent(skyboxGraphicsComponent);
  skyboxActor->ShouldTranslateWithView(false);

  AddGameObject(skyboxActor);
}

ZCamera* ZGame::GetActiveCamera() const {
  assert(activeCameraIndex_ >= 0 && activeCameraIndex_ < gameCameras_.size()); // TODO: remove or enable only for debug builds
  return gameCameras_[activeCameraIndex_];
}

void ZGame::HandleEscape() {
  ZEngine::Domain()->Strategy()->ReleaseCursor();
}

// -.-
void ZGame::MacDisplayHack() {
  #ifdef __APPLE__
  static bool moved = false;
  if (!moved) {
    ZEngine::Domain()->ResizeWindow(
      ZEngine::Domain()->WindowWidth() + 1,
      ZEngine::Domain()->WindowHeight() + 1
    );
    moved = true;
  }
  #endif
}
