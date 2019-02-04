//
//  ZCamera.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include <glm/glm.hpp>

// Forward Declarations

// Class and Data Structure Definitions
enum ZCameraType {
  Orthographic, Perspective
};

class ZCamera : public ZGameObject {
private:
  float movementSpeed_ = 2.2f;
  float lookSensitivity_ = 0.1f;
  float zoom_ = 45.0f;
  float zoomSpeed_ = 5.0f;
  float nearClippingPlane_ = 0.01f;
  float farClippingPlane_ = 1000.0f;
  ZCameraType cameraType_;

public:
  ZCamera(ZCameraType type = ZCameraType::Orthographic, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.f, -90.f, 0.f))
  : ZGameObject(position, rotation) {
    cameraType_ = type;
    zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
  }

  virtual void Update() override { }
  virtual void Render(float frameMix) override { };

  virtual void HandleStrafe(float controlThrow) override;
  virtual void HandleForwardBack(float controlThrow) override;
  virtual void HandlePitch(float controlThrow) override;
  virtual void HandleYaw(float controlThrow) override;

  ZCameraType Type() { return cameraType_; }
  void SetType(ZCameraType type) { cameraType_ = type; }

  virtual glm::mat4 GetViewMatrix();
  float GetZoom() const { return zoom_; }
  float GetNearField() const { return nearClippingPlane_; }
  float GetFarField() const { return farClippingPlane_; }

protected:

};
