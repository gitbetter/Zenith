//
//  ZCameraComponent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZComponent.hpp"

// Forward Declarations
class ZPhysicsComponent;
struct ZOFNode;

// Class and Data Structure Definitions
class ZCameraComponent : public ZComponent {
private:
  float movementSpeed_ = 10.f;
  float lookSensitivity_ = 0.1f;
  float zoom_ = 45.0f;
  float zoomSpeed_ = 5.0f;
  float nearClippingPlane_ = 0.01f;
  float farClippingPlane_ = 1000.0f;
  ZCameraType cameraType_;
  ZCameraMovementStyle movementStyle_;
  float cameraDamping_ = 0.02f;

  glm::vec3 pitchVelocity_, yawVelocity_;
  glm::quat pitch_, yaw_;

  ZPhysicsComponent* AddPhysicsComponentIfNeeded();

public:
  ZCameraComponent(ZCameraType type = ZCameraType::Orthographic, glm::vec3 position = glm::vec3(0.0f)) : ZComponent() {
    cameraType_ = type;
    zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
    movementStyle_ = ZCameraMovementStyle::Normal;
    pitch_ = glm::quat(0.f, glm::vec3(1.f, 0.f, 0.f));
    yaw_ = glm::quat(0.f, glm::vec3(0.f, 1.f, 0.f));
    pitchVelocity_ = glm::vec3(0.f);
    yawVelocity_ = glm::vec3(0.f);
  }
  ~ZCameraComponent() { }

  void Initialize(ZOFNode* root) override;

  void UpdateCameraOrientation();

  void HandleStrafe(float controlThrow) override;
  void HandleForwardBack(float controlThrow) override;
  void HandlePitch(float controlThrow) override;
  void HandleYaw(float controlThrow) override;

  ZCameraType Type() { return cameraType_; }
  void SetType(ZCameraType type) { cameraType_ = type; }
  void SetMovementStyle(ZCameraMovementStyle style) { movementStyle_ = style; }

  glm::mat4 ViewMatrix(float frameMix);
  float GetZoom() const { return zoom_; }
  float GetNearField() const { return nearClippingPlane_; }
  float GetFarField() const { return farClippingPlane_; }

protected:
};
