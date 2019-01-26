//
//  ZEngine.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGameObject.hpp"

#include <chrono>
using namespace std;

const float ZEngine::MS_PER_UPDATE = 17.0f;
const int ZEngine::MAX_FIXED_UPDATE_ITERATIONS = 50;
bool ZEngine::running_ = true;
std::vector<ZGameObject*> ZEngine::gameObjects_;

void ZEngine::RunGameLoop() {
    float previousTime = chrono::high_resolution_clock::now().time_since_epoch().count();
    float lag = 0.0f;
    while (running_) {
        int fixedUpdates = 0;
        float currentTime = chrono::high_resolution_clock::now().time_since_epoch().count();
        float elapsedTime = currentTime - previousTime;
        lag += elapsedTime;

        while (lag >= MS_PER_UPDATE && ++fixedUpdates <= MAX_FIXED_UPDATE_ITERATIONS) {
            Update();
            lag -= MS_PER_UPDATE;
        }

        Render(lag / MS_PER_UPDATE);
    }
}

void ZEngine::Update() {
    for (unsigned int i = 0; i < gameObjects_.size(); i++) {
        gameObjects_[i]->Update();
    }
}

void ZEngine::Render(float frameMix) {
    for (unsigned int i = 0; i < gameObjects_.size(); i++) {
        gameObjects_[i]->Render(frameMix);
    }
}
