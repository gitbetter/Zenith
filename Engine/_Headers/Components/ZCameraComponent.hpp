/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZCameraComponent.hpp

    Created by Adrian Sanchez on 16/02/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZComponent.hpp"
#include "ZFrustum.hpp"

// Forward Declarations
class ZPhysicsComponent;
struct ZOFNode;

// Class and Data Structure Definitions
class ZCameraComponent : public ZComponent {
  
private:

  float movementSpeed_ = 25.f;
  float lookSensitivity_ = 0.1f;
  float zoom_ = 45.0f;
  float zoomSpeed_ = 5.0f;
  float nearClippingPlane_ = 0.01f;
  float farClippingPlane_ = 1000.0f;
  float cameraDamping_ = 0.02f;
	ZCameraType cameraType_;
	ZCameraMovementStyle movementStyle_;
	ZFrustum frustum_;

  glm::vec3 pitchVelocity_, yawVelocity_;
  glm::quat pitch_, yaw_;

  void HandleMove(std::shared_ptr<ZEvent> event);
  void HandleLook(std::shared_ptr<ZEvent> event);

public:

	ZCameraComponent(ZCameraType type = ZCameraType::Orthographic, glm::vec3 position = glm::vec3(0.0f));
  ~ZCameraComponent() { }

  void Initialize(std::shared_ptr<ZOFNode> root) override;

  void Update() override;

  void UpdateCameraOrientation();

  ZCameraType Type() { return cameraType_; }
  void SetType(ZCameraType type) { cameraType_ = type; }
  void SetMovementStyle(ZCameraMovementStyle style) { movementStyle_ = style; }

  float Zoom() const { return zoom_; }
  float NearField() const { return nearClippingPlane_; }
  float FarField() const { return farClippingPlane_; }
	ZFrustum& Frustum() { return frustum_; }
  glm::mat4 ProjectionMatrix();
  glm::mat4 ViewMatrix();

protected:

};
