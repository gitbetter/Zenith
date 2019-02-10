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

void ZCamera::Update() {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    eulerRotation_.x = glm::clamp(eulerRotation_.x + eulerVelocity_.x * ZEngine::UPDATE_STEP_SIZE, -89.0f, 89.0f);
    eulerRotation_.y += eulerVelocity_.y * ZEngine::UPDATE_STEP_SIZE;

    eulerVelocity_ *= glm::pow(eulerDamping_, ZEngine::UPDATE_STEP_SIZE);
  }
  UpdateFrontVectorRotation();
}

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

  if (cameraType_ == ZCameraType::Orthographic) {
    zoom_ += zoomSpeed_ * controlThrow * velocity;
  }
}

void ZCamera::HandlePitch(float controlThrow) {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    eulerVelocity_.x += controlThrow * lookSensitivity_;
  } else {
    eulerRotation_.x = glm::clamp(eulerRotation_.x + controlThrow * lookSensitivity_, -89.0f, 89.0f);
  }
}

void ZCamera::HandleYaw(float controlThrow) {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    eulerVelocity_.y += controlThrow * lookSensitivity_;
  } else {
    eulerRotation_.y += controlThrow * lookSensitivity_;
  }
}
