//
//  ZCameraComponent.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZCameraComponent.hpp"
#include "ZCommon.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZOFTree.hpp"

void ZCameraComponent::Update() {
  UpdateCameraOrientation();
}

void ZCameraComponent::UpdateCameraOrientation() {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    pitchVelocity_ *= glm::pow(cameraDamping_, ZEngine::UPDATE_STEP_SIZE);
    yawVelocity_ *= glm::pow(cameraDamping_, ZEngine::UPDATE_STEP_SIZE);
    pitch_ = glm::quat(pitchVelocity_ * ZEngine::UPDATE_STEP_SIZE);
    yaw_ = glm::quat(yawVelocity_ * ZEngine::UPDATE_STEP_SIZE);
    object_->SetOrientation(glm::normalize(pitch_ * object_->Orientation() * yaw_));
  }
}

// TODO: These initalize functions can get pretty hectic. Maybe there's a better way...
void ZCameraComponent::Initialize(ZOFNode* root) {
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZCameraComponent", ZERROR);
    return;
  }

  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() == 0) continue;

    if (prop->id == "speed") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      movementSpeed_ = terminal->value;
    } else if (prop->id == "sensitivity") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      lookSensitivity_ = terminal->value;
    } else if (prop->id == "zoom") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      zoom_ = terminal->value;
    } else if (prop->id == "zoomSpeed") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      zoomSpeed_ = terminal->value;
    } else if (prop->id == "nearPlane") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      nearClippingPlane_ = terminal->value;
    } else if (prop->id == "farPlane") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      farClippingPlane_ = terminal->value;
    } else if (prop->id == "type") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      cameraType_ = terminal->value == "Orthographic" ? ZCameraType::Orthographic : ZCameraType::Perspective;
      zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
    } else if (prop->id == "movementStyle") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      movementStyle_ = terminal->value == "Follow" ? ZCameraMovementStyle::Follow : ZCameraMovementStyle::Normal;
    } else if (prop->id == "damping") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      cameraDamping_ = terminal->value;
    }
  }
}

void ZCameraComponent::HandleStrafe(float controlThrow) {
  float velocity = movementSpeed_ * ZEngine::DeltaTime();
  object_->SetPosition(object_->Position() + object_->Right() * controlThrow * -velocity);
}

void ZCameraComponent::HandleForwardBack(float controlThrow) {
  float velocity = movementSpeed_ * ZEngine::DeltaTime();
  object_->SetPosition(object_->Position() + object_->Front() * controlThrow * velocity);

  if (cameraType_ == ZCameraType::Orthographic) {
    zoom_ += zoomSpeed_ * controlThrow * velocity;
  }
}

void ZCameraComponent::HandlePitch(float controlThrow) {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    pitchVelocity_ += glm::vec3(glm::radians(-controlThrow * lookSensitivity_), 0.f, 0.f);
  } else if (movementStyle_ == ZCameraMovementStyle::Normal) {
    pitch_ = glm::angleAxis(glm::radians(-controlThrow * lookSensitivity_), glm::vec3(1.f, 0.f, 0.f));
    object_->SetOrientation(glm::normalize(pitch_ * object_->Orientation()));
  }
}

void ZCameraComponent::HandleYaw(float controlThrow) {
  if (movementStyle_ == ZCameraMovementStyle::Follow) {
    yawVelocity_ += glm::vec3(0.f, glm::radians(controlThrow * lookSensitivity_), 0.f);
  } else if (movementStyle_ == ZCameraMovementStyle::Normal) {
    yaw_ = glm::angleAxis(glm::radians(controlThrow * lookSensitivity_), glm::vec3(0.f, 1.f, 0.f));
    object_->SetOrientation(glm::normalize(object_->Orientation() * yaw_));
  }
}

glm::mat4 ZCameraComponent::ViewMatrix(float frameMix) {
  glm::vec3 interpolatedFront = object_->PreviousFront() * (1.f - frameMix) + object_->Front() * frameMix;
  glm::vec3 interpolatedUp = object_->PreviousUp() * (1.f - frameMix) + object_->Up() * frameMix;
  return glm::lookAt(glm::vec3(object_->Position()),
                     glm::vec3(object_->Position()) + interpolatedFront,
                     interpolatedUp);
}
