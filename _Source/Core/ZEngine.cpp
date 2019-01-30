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

const float ZEngine::MS_PER_UPDATE = 17.0f;
const int ZEngine::MAX_FIXED_UPDATE_ITERATIONS = 50;
const glm::vec3 ZEngine::WORLD_UP(0.0f, 1.0f, 0.0f);

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
