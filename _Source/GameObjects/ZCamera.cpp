//
//  ZCamera.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZCamera.hpp"
#include "ZLogger.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 ZCamera::GetViewMatrix() {
  return glm::lookAt(glm::vec3(position_), glm::vec3(position_ + front_), glm::vec3(up_));
}

void ZCamera::HandleStrafe(float controlThrow) {
  float velocity = movementSpeed_ * ZEngine::DeltaTime();
  position_ += right_ * controlThrow * velocity;
}

void ZCamera::HandleForwardBack(float controlThrow) {
  float velocity = movementSpeed_ * ZEngine::DeltaTime();
  position_ += front_ * controlThrow * velocity;
}

void ZCamera::HandlePitch(float controlThrow) {
  float pitch = controlThrow * lookSensitivity_;
  eulerRotation_.x = glm::clamp(eulerRotation_.x + pitch, -89.0f, 89.0f);
  UpdateFrontVectorRotation();
}

void ZCamera::HandleYaw(float controlThrow) {
  float yaw = controlThrow * lookSensitivity_;
  eulerRotation_.y += yaw;
  UpdateFrontVectorRotation();
}
