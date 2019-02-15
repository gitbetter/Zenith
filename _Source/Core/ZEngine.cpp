//
//  ZEngine.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZNullInput.hpp"
#include "ZUI.hpp"
#include "ZPhysics.hpp"
#include <chrono>

const float ZEngine::UPDATE_STEP_SIZE = 0.017f;
const int ZEngine::MAX_FIXED_UPDATE_ITERATIONS = 50;
const unsigned int ZEngine::SHADOW_MAP_SIZE = 1024;
const glm::vec4 ZEngine::WORLD_UP(0.0f, 1.0f, 0.0f, 0.0f);
const std::vector<std::string> ZEngine::DEFAULT_SKYBOX_CUBEMAP{
  "Assets/Skyboxes/Default/right.png",
  "Assets/Skyboxes/Default/left.png",
  "Assets/Skyboxes/Default/up.png",
  "Assets/Skyboxes/Default/down.png",
  "Assets/Skyboxes/Default/front.png",
  "Assets/Skyboxes/Default/back.png",
};

ZDomain* ZEngine::domain_ = nullptr;
ZGraphics* ZEngine::graphics_ = nullptr;
ZInput* ZEngine::input_ = new ZNullInput;
ZUI* ZEngine::ui_ = nullptr;
ZPhysics* ZEngine::physics_ = nullptr;
float ZEngine::deltaTime_ = 0.0f;

ZDomain* ZEngine::Domain() {
  return domain_;
}

ZGraphics* ZEngine::Graphics() {
  return graphics_;
}

ZInput* ZEngine::Input() {
  return input_;
}

ZUI* ZEngine::UI() {
  return ui_;
}

ZPhysics* ZEngine::Physics() {
  return physics_;
}

float ZEngine::DeltaTime() {
  return deltaTime_;
}

float ZEngine::MilliSecondTime() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() / 1000.0f;
}

void ZEngine::Provide(ZGraphics& graphics) {
  if (graphics_ != nullptr) {
    delete graphics_;
  }
  graphics_ = &graphics;
  graphics_->Initialize();
}

void ZEngine::Provide(ZDomain& domain) {
  if (domain_ != nullptr) {
    delete domain_;
  }
  domain_ = &domain;
  domain_->Initialize();
}

void ZEngine::Provide(ZInput& input) {
  // If the provided input object is not null and the existing engine input object
  // is null, delete the existing one
  if (!dynamic_cast<ZNullInput*>(&input) && dynamic_cast<ZNullInput*>(input_)) {
    delete input_;
  }
  input_ = &input;
}

void ZEngine::Provide(ZUI& ui) {
  if (ui_ != nullptr) {
    delete ui_;
  }
  ui_ = &ui;
  ui_->Initialize();
}

void ZEngine::Provide(ZPhysics& ui) {
  if (physics_ != nullptr) {
    delete physics_;
  }
  physics_ = &ui;
  physics_->Initialize();
}

void ZEngine::SetDeltaTime(float deltaTime) {
  deltaTime_ = deltaTime;
}
