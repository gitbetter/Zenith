//
//  ZCameraComponent.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZCameraComponent.hpp"
#include "ZCommon.hpp"
#include "ZDomain.hpp"
#include "ZGameObject.hpp"
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

  ZOFPropertyMap props = node->properties;

  if (props.find("speed") != props.end() && props["speed"]->HasValues()) {
    ZOFNumber* prop = props["speed"]->Value<ZOFNumber>(0);
    movementSpeed_ = prop->value;
  }

  if (props.find("sensitivity") != props.end() && props["sensitivity"]->HasValues()) {
    ZOFNumber* prop = props["sensitivity"]->Value<ZOFNumber>(0);
    lookSensitivity_ = prop->value;
  }

  if (props.find("zoom") != props.end() && props["zoom"]->HasValues()) {
    ZOFNumber* prop = props["zoom"]->Value<ZOFNumber>(0);
    zoom_ = prop->value;
  }

  if (props.find("zoomSpeed") != props.end() && props["zoomSpeed"]->HasValues()) {
    ZOFNumber* prop = props["zoomSpeed"]->Value<ZOFNumber>(0);
    zoomSpeed_ = prop->value;
  }

  if (props.find("nearPlane") != props.end() && props["nearPlane"]->HasValues()) {
    ZOFNumber* prop = props["nearPlane"]->Value<ZOFNumber>(0);
    nearClippingPlane_ = prop->value;
  }

  if (props.find("farPlane") != props.end() && props["farPlane"]->HasValues()) {
    ZOFNumber* prop = props["farPlane"]->Value<ZOFNumber>(0);
    farClippingPlane_ = prop->value;
  }

  if (props.find("type") != props.end() && props["type"]->HasValues()) {
    ZOFString* prop = props["type"]->Value<ZOFString>(0);
    cameraType_ = prop->value == "Orthographic" ? ZCameraType::Orthographic : ZCameraType::Perspective;
    zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
  }

  if (props.find("movementStyle") != props.end() && props["movementStyle"]->HasValues()) {
    ZOFString* prop = props["movementStyle"]->Value<ZOFString>(0);
    movementStyle_ = prop->value == "Follow" ? ZCameraMovementStyle::Follow : ZCameraMovementStyle::Normal;
  }

  if (props.find("damping") != props.end() && props["damping"]->HasValues()) {
    ZOFNumber* prop = props["damping"]->Value<ZOFNumber>(0);
    cameraDamping_ = prop->value;
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

glm::mat4 ZCameraComponent::ProjectionMatrix() {
  const ZDomain* domain = ZEngine::Domain();
  glm::mat4 projectionMatrix;
  if (cameraType_ == ZCameraType::Orthographic) {
    float left = -(float)domain->ResolutionX() / (zoom_ * 2);
    float right = -left;
    float bottom = -(float)domain->ResolutionY() / (zoom_ * 2);
    float top = -bottom;
    projectionMatrix = glm::ortho(left, right, bottom, top, -farClippingPlane_ / 2.f, farClippingPlane_);
  } else {
    projectionMatrix = glm::perspective(glm::radians(zoom_),
                                         (float)domain->ResolutionX() / (float)domain->ResolutionY(),
                                         nearClippingPlane_, farClippingPlane_);
  }
  return projectionMatrix;
}

glm::mat4 ZCameraComponent::ViewMatrix(float frameMix) {
  glm::vec3 interpolatedFront = object_->PreviousFront() * (1.f - frameMix) + object_->Front() * frameMix;
  glm::vec3 interpolatedUp = object_->PreviousUp() * (1.f - frameMix) + object_->Up() * frameMix;
  return glm::lookAt(glm::vec3(object_->Position()),
                     glm::vec3(object_->Position()) + interpolatedFront,
                     interpolatedUp);
}
