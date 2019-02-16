//
//  ZCameraComponent.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZCameraComponent.hpp"

//
//  ZCamera.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZCameraComponent.hpp"
#include "ZCommon.hpp"

glm::mat4 ZCameraComponent::ViewMatrix(float frameMix) {
  return glm::lookAt(glm::vec3(object_->Position()), glm::vec3(object_->Position() + object_->Front()), glm::vec3(object_->Up());
}

void ZCameraComponent::HandleStrafe(float controlThrow) {
  float velocity = movementSpeed_ * ZEngine::DeltaTime();
  object_->SetPosition(object_->Position() + object_->Right() * controlThrow * velocity)
}

void ZCameraComponent::HandleForwardBack(float controlThrow) {
  float velocity = movementSpeed_ * ZEngine::DeltaTime();
  object_->SetPosition(object_->Position() + object_->Front() * controlThrow * velocity)

  if (cameraType_ == ZCameraType::Orthographic) {
    zoom_ += zoomSpeed_ * controlThrow * velocity;
  }
}

void ZCameraComponent::HandlePitch(float controlThrow) {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    eulerVelocity_.x += controlThrow * lookSensitivity_;
  } else if (movementStyle_ == ZCameraMovementStyle::Normal) {
    glm::vec3 euler = glm::eulerAngles(object_->Orientation());
    euler.x = glm::clamp(euler.x + controlThrow * lookSensitivity_, -89.0f, 89.0f);
    object_->SetOrientation(glm::quat(euler));
  }
}

void ZCameraComponent::HandleYaw(float controlThrow) {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    eulerVelocity_.y += controlThrow * lookSensitivity_;
  } else if (movementStyle_ == ZCameraMovementStyle::Normal) {
    glm::vec3 euler = glm::eulerAngles(object_->Orientation());
    euler.y = euler.y + controlThrow * lookSensitivity_;
    object_->SetOrientation(glm::quat(euler))
  }
}
