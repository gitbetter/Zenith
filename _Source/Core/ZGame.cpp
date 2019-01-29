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
#include "ZGameObject.hpp"
#include "ZLogger.hpp"

#include <chrono>
using namespace std;

ZGame::ZGame() { }

void ZGame::RunGameLoop() {
  ZLogger::Log("Zenith is about to loop...", ZLoggerSeverity::Info);
  float previousTime = chrono::high_resolution_clock::now().time_since_epoch().count();
  float lag = 0.0f;
  while (!ZEngine::GetGraphics()->GetWindow()->WindowShouldClose()) {
    ZEngine::GetInput()->ProcessInput();

    int fixedUpdates = 0;
    float currentTime = chrono::high_resolution_clock::now().time_since_epoch().count();
    float elapsedTime = currentTime - previousTime;
    lag += elapsedTime;

    while (lag >= ZEngine::MS_PER_UPDATE && ++fixedUpdates <= ZEngine::MAX_FIXED_UPDATE_ITERATIONS) {
        Update();
        lag -= ZEngine::MS_PER_UPDATE;
    }

    Render(lag / ZEngine::MS_PER_UPDATE);
    ZEngine::GetGraphics()->GetWindow()->PollEvents();
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

void ZGame::HandleUpDown(float controlThrow) {
  ZLogger::Log("UpDown key pressed", ZLoggerSeverity::Info);
}
