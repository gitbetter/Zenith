//
//  ZCamera.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGameObject.hpp"
#include <glm/glm.hpp>

// Forward Declarations
class ZInputComponent;

// Class and Data Structure Definitions
class ZCamera : public ZGameObject {
private:
  float movementSpeed_ = 3.0f;
  float lookSensitivity_ = 0.1f;
  float zoom_ = 45.0f;
  float nearClippingPlane_ = 0.01f;
  float farClippingPlane_ = 100.0f;

public:
  ZCamera(glm::vec3 position = glm::vec3(0.0f)) : ZGameObject(position) { }

  virtual void Update() override { }
  virtual void Render(float frameMix) override { };

  virtual void HandleStrafe(float controlThrow) override;
  virtual void HandleForwardBack(float controlThrow) override;
  virtual void HandlePitch(float controlThrow) override;
  virtual void HandleYaw(float controlThrow) override;

  virtual glm::mat4 GetViewMatrix();
  float GetZoom() const { return zoom_; }
  float GetNearField() const { return nearClippingPlane_; }
  float GetFarField() const { return farClippingPlane_; }

protected:

};
