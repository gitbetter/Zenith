//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include "ZCommon.hpp"

void ZGameObject::SetFrontVector(glm::vec3 front) {
  front_ = glm::normalize(glm::vec4(front, 1.f));
  previousFront_ = front_;
  CalculateTangentBasis();
}

void ZGameObject::UpdateFrontVectorRotation() {
  previousFront_ = front_;
  glm::vec4 front(0.f);
  front.x = glm::cos(glm::radians(eulerRotation_.y)) * glm::cos(glm::radians(eulerRotation_.x));
  front.y = glm::sin(glm::radians(eulerRotation_.x));
  front.z = glm::sin(glm::radians(eulerRotation_.y)) * glm::cos(glm::radians(eulerRotation_.x));
  front_ = glm::normalize(front);
  CalculateTangentBasis();
}

void ZGameObject::CalculateTangentBasis() {
  previousRight_ = right_; previousUp_ = up_;
  right_ = glm::vec4(glm::normalize(glm::cross(glm::vec3(front_), glm::vec3(ZEngine::WORLD_UP))), 0.0f);
  up_ = glm::vec4(glm::normalize(glm::cross(glm::vec3(right_), glm::vec3(front_))), 0.0f);
}

glm::mat4 ZGameObject::ViewMatrix(float frameMix) {
  glm::vec4 interpolatedFront = previousFront_ * (1.f - frameMix) + front_ * frameMix;
  glm::vec4 interpolatedUp = previousUp_ * (1.f - frameMix) + up_ * frameMix;
  return glm::lookAt(glm::vec3(position_), glm::vec3(position_ + interpolatedFront), glm::vec3(interpolatedUp));
}
