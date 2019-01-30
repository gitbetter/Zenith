//
//  ZCamera.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZCamera.hpp"
#include "ZLogger.hpp"
#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 ZCamera::GetViewMatrix() {
  return glm::lookAt(position_, position_ + front_, up_);
}

void ZCamera::HandleStrafe(float controlThrow) {
  position_.x += controlThrow * movementSpeed_ * ZEngine::DeltaTime();
}

void ZCamera::HandleForwardBack(float controlThrow) {
  position_.z -= controlThrow * movementSpeed_ * ZEngine::DeltaTime();
}

void ZCamera::HandlePitch(float controlThrow) {
  float pitch = -controlThrow * lookSensitivity_;
  glm::quat deltaRotation(glm::vec3(pitch, 0.0f, 0.0f));
  front_ = glm::vec3(glm::mat4_cast(deltaRotation) * glm::vec4(front_, 1.0f));
}

void ZCamera::HandleYaw(float controlThrow) {
  float yaw = -controlThrow * lookSensitivity_;
  glm::quat deltaRotation(glm::vec3(0.0f, yaw, 0.0f));
  front_ = glm::vec3(glm::mat4_cast(deltaRotation) * glm::vec4(front_, 1.0f));
}
