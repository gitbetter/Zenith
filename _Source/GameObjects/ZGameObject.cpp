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

ZGameObject::ZGameObject(glm::vec3 position, glm::vec3 orientation)
: position_(glm::vec4(position, 1.f)),
  orientation_(orientation),
  modelMatrix_(glm::mat4(1.f)),
  translatesWithView_(false),
  eulerVelocity_(glm::vec4(0.f)),
  eulerDamping_(0.05f)
{
  id_ = "ZGO_" + std::to_string(ZEngine::NewId());
  CalculateModelMatrix();
}

void ZGameObject::ShouldTranslateWithView(bool translates) {
  translatesWithView_ = translates;
}

void ZGameObject::SetPosition(glm::vec3 position) {
  position_ = glm::vec4(position, 1.f);
  CalculateModelMatrix();
}

void ZGameObject::SetOrientation(glm::quat quaternion) {
  orientation_ = quaternion;
  CalculateModelMatrix();
}

void ZGameObject::SetOrientation(glm::vec3 euler) {
  orientation_ = glm::quat(euler);
  CalculateModelMatrix();
}

void ZGameObject::CalculateModelMatrix() {
  modelMatrix_ = glm::rotate(modelMatrix_, glm::angle(orientation_), glm::axis(orientation_));
  modelMatrix_ = glm::translate(modelMatrix_, position_);
}
