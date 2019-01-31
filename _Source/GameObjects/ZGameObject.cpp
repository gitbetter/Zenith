//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include "ZLogger.hpp"

#include <glm/glm.hpp>

void ZGameObject::SetFrontVector(glm::vec4 front) {
  front_ = glm::normalize(front);
  CalculateTangentBasis();
}

void ZGameObject::UpdateFrontVectorRotation() {
  glm::vec4 front;
  front.x = glm::cos(glm::radians(eulerRotation_.y)) * glm::cos(glm::radians(eulerRotation_.x));
  front.y = glm::sin(glm::radians(eulerRotation_.x));
  front.z = glm::sin(glm::radians(eulerRotation_.y)) * glm::cos(glm::radians(eulerRotation_.x));
  front_ = glm::normalize(front);
  CalculateTangentBasis();
}

void ZGameObject::CalculateTangentBasis() {
  right_ = glm::vec4(glm::normalize(glm::cross(glm::vec3(front_), glm::vec3(ZEngine::WORLD_UP))), 0.0f);
  up_ = glm::vec4(glm::normalize(glm::cross(glm::vec3(right_), glm::vec3(front_))), 0.0f);
}
