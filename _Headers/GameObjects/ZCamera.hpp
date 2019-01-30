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
  float zoom_ = 45.0f;
  float nearClippingPlane_ = 0.01f;
  float farClippingPlane_ = 100.0f;

public:
  ZCamera() : ZGameObject() { }
  
  virtual void Update() override { }
  virtual void Render(float frameMix) override { };

  virtual glm::mat4 GetViewMatrix();
  float GetZoom() const { return zoom_; }
  float GetNearField() const { return nearClippingPlane_; }
  float GetFarField() const { return farClippingPlane_; }

protected:

};
