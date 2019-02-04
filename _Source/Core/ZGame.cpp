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
#include "ZWindow.hpp"
#include "ZInput.hpp"
#include "ZCamera.hpp"
#include "ZGameObject.hpp"
#include "ZLogger.hpp"

#include <chrono>
#include <cassert>
using namespace std;

ZGame::ZGame() : activeCameraIndex_(-1) { }

void ZGame::RunGameLoop() {
  ZLogger::Log("Zenith is about to loop...", ZLoggerSeverity::Info);

  float previousTime = ZEngine::MilliSecondTime();
  float lag = 0.0;

  while (!ZEngine::GetGraphics()->GetWindow()->WindowShouldClose()) {
    int fixedUpdates = 0;
    float currentTime = ZEngine::MilliSecondTime();
    ZEngine::SetDeltaTime(currentTime - previousTime);
    previousTime = currentTime;
    lag += ZEngine::DeltaTime();


    ZEngine::GetInput()->ProcessInput();

    while (lag >= ZEngine::MS_PER_UPDATE && ++fixedUpdates <= ZEngine::MAX_FIXED_UPDATE_ITERATIONS) {
        Update();
        lag -= ZEngine::MS_PER_UPDATE;
    }

    Render(lag / ZEngine::MS_PER_UPDATE);
    ZEngine::GetGraphics()->GetWindow()->PollEvents();
    MacDisplayHack();
  }
}

void ZGame::Update() {
  for (unsigned int i = 0; i < gameObjects_.size(); i++) {
      gameObjects_[i]->Update();
  }
}

void ZGame::Render(float frameMix) {
  ZEngine::GetGraphics()->Draw(gameObjects_, frameMix);
}

void ZGame::AddGameObject(ZGameObject* gameObject) {
  if (gameObject != nullptr) {
    gameObject->game_ = this;
    if (dynamic_cast<ZCamera*>(gameObject)) {
      gameCameras_.push_back(dynamic_cast<ZCamera*>(gameObject));
      activeCameraIndex_ += 1;
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

ZCamera* ZGame::GetActiveCamera() const {
  assert(activeCameraIndex_ >= 0 && activeCameraIndex_ < gameCameras_.size()); // TODO: remove
  return gameCameras_[activeCameraIndex_];
}

void ZGame::HandleEscape() {
  ZEngine::GetGraphics()->GetWindow()->CloseWindow();
}

// -.-
void ZGame::MacDisplayHack() {
  #ifdef __APPLE__
  static bool moved = false;
  if (!moved) {
    ZEngine::GetGraphics()->GetWindow()->SetSize(
      ZEngine::GetGraphics()->GetWindow()->GetWidth() + 1,
      ZEngine::GetGraphics()->GetWindow()->GetHeight() + 1
    );
    moved = true;
  }
  #endif
}
