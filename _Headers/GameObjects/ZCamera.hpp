//
//  ZCamera.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include <glm/glm.hpp>

// Forward Declarations

// Class and Data Structure Definitions
class ZCamera : public ZGameObject {
private:
  float movementSpeed_ = 2.2f;
  float lookSensitivity_ = 0.1f;
  float zoom_ = 45.0f;
  float zoomSpeed_ = 5.0f;
  float nearClippingPlane_ = 0.01f;
  float farClippingPlane_ = 1000.0f;
  ZCameraType cameraType_;
  ZCameraMovementStyle movementStyle_;

public:
  ZCamera(ZCameraType type = ZCameraType::Orthographic, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.f, -90.f, 0.f))
  : ZGameObject(position, rotation) {
    cameraType_ = type;
    zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
    movementStyle_ = ZCameraMovementStyle::Normal;
  }

  void Update() override;
  void Render(float frameMix, unsigned char renderOp = ZGraphics::RENDER_OP_COLOR) override { };

  void HandleStrafe(float controlThrow) override;
  void HandleForwardBack(float controlThrow) override;
  void HandlePitch(float controlThrow) override;
  void HandleYaw(float controlThrow) override;

  ZCameraType Type() { return cameraType_; }
  void SetType(ZCameraType type) { cameraType_ = type; }
  void SetMovementStyle(ZCameraMovementStyle style) { movementStyle_ = style; }

  glm::mat4 ViewMatrix();
  float GetZoom() const { return zoom_; }
  float GetNearField() const { return nearClippingPlane_; }
  float GetFarField() const { return farClippingPlane_; }

protected:

};
