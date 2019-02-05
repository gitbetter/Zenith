//
//  ZEngine.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZNullGraphics.hpp"
#include "ZNullInput.hpp"
#include <chrono>

const float ZEngine::MS_PER_UPDATE = 17.0f;
const int ZEngine::MAX_FIXED_UPDATE_ITERATIONS = 50;
const unsigned int ZEngine::SHADOW_MAP_WIDTH = 1024;
const unsigned int ZEngine::SHADOW_MAP_HEIGHT = 1024;
const glm::vec4 ZEngine::WORLD_UP(0.0f, 1.0f, 0.0f, 0.0f);
const std::vector<std::string> ZEngine::DEFAULT_SKYBOX_CUBEMAP{
  "Resources/Skyboxes/Default/right.tga",
  "Resources/Skyboxes/Default/left.tga",
  "Resources/Skyboxes/Default/up.tga",
  "Resources/Skyboxes/Default/down.tga",
  "Resources/Skyboxes/Default/front.tga",
  "Resources/Skyboxes/Default/back.tga",
};

ZGraphics* ZEngine::graphics_ = new ZNullGraphics;
ZInput* ZEngine::input_ = new ZNullInput;
float ZEngine::deltaTime_ = 0.0f;

ZGraphics* ZEngine::GetGraphics() {
  return graphics_;
}

ZInput* ZEngine::GetInput() {
  return input_;
}

float ZEngine::DeltaTime() {
  return deltaTime_;
}

float ZEngine::MilliSecondTime() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() / 1000.0f;
}

void ZEngine::Provide(ZGraphics& graphics) {
  // If the provided graphics object is not null and the existing engine graphics object
  // is null, delete the existing one
  if (!dynamic_cast<ZNullGraphics*>(&graphics) && dynamic_cast<ZNullGraphics*>(graphics_)) {
    delete graphics_;
  }
  graphics_ = &graphics;
}

void ZEngine::Provide(ZInput& input) {
  // If the provided input object is not null and the existing engine input object
  // is null, delete the existing one
  if (!dynamic_cast<ZNullInput*>(&input) && dynamic_cast<ZNullInput*>(input_)) {
    delete input_;
  }
  input_ = &input;
}

void ZEngine::SetDeltaTime(float deltaTime) {
  deltaTime_ = deltaTime;
}
