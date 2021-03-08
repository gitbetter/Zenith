/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZCamera.hpp

   Created by Adrian Sanchez on 26/01/2021.
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
#include "ZGameObject.hpp"
#include "ZFrustum.hpp"

 // Forward Declarations
struct ZOFNode;
class ZLookEvent;
class ZMoveEvent;
class ZUniformBuffer;
class ZRenderStateGroup;

// Class and Data Structure Definitions

class ZCamera : public ZGameObject
{

public:

    ZCamera(const glm::vec3& position = glm::vec3(0.f), const glm::quat& orientation = glm::quat(glm::vec3(0.f)), const glm::vec3& scale = glm::vec3(1.f), ZCameraType type = ZCameraType::Orthographic);
    ZCamera::ZCamera(ZCameraType type) 
        : ZCamera(glm::vec3(0.f), glm::quat(glm::vec3(0.f)), glm::vec3(1.f), type)
    { }
    ~ZCamera() {}

    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override;

    void Prepare(double deltaTime) override;
    bool IsVisible() override { return true; }

    std::shared_ptr<ZGameObject> Clone() override;
    void CleanUp() override;

    ZCameraType Type() { return cameraType_; }
    float Zoom() const { return zoom_; }
    float Speed() const { return movementSpeed_; }
    float Sensitivity() const { return lookSensitivity_; }
    float NearField() const { return nearClippingPlane_; }
    float FarField() const { return farClippingPlane_; }
    ZFrustum Frustum() const { return frustum_; }
    bool IsPrimary() const { return isPrimary_; }
    bool Moving() const { return moving_; }
    glm::mat4 ProjectionMatrix() { return projection_; }
    glm::mat4 ViewMatrix() { return view_; }
    glm::mat4 ViewProjectionMatrix() { return viewProjection_; }
    glm::mat4 InverseViewProjectionMatrix() { return inverseViewProjection_; }
    glm::mat4 PreviousViewProjectionMatrix() { return previousViewProjection_; }
    const std::shared_ptr<ZRenderStateGroup>& RenderState() const { return renderState_; }

    void SetType(ZCameraType type) { cameraType_ = type; }
    void SetSpeed(float speed) { movementSpeed_ = speed; }
    void SetSensitivity(float sensitivity) { lookSensitivity_ = sensitivity; }
    void SetMovementStyle(ZCameraMovementStyle style) { movementStyle_ = style; }

    void Move(float z, float x, bool useWorldFront = false);
    void EnableMovement() { movementEnabled_ = true; }
    void DisableMovement() { movementEnabled_ = false; }

    void Look(float pitch, float yaw);
    void EnableLook() { lookEnabled_ = true; }
    void DisableLook() { lookEnabled_ = false; }

    DECLARE_OBJECT_CREATORS(ZCamera)

private:

    bool isPrimary_ = false;
    bool movementEnabled_ = true;
    bool lookEnabled_ = true;
    bool moving_ = true;
    float movementSpeed_ = 25.f;
    float lookSensitivity_ = 0.1f;
    float zoom_ = 45.0f;
    float zoomSpeed_ = 5.0f;
    float nearClippingPlane_ = 0.01f;
    float farClippingPlane_ = 100.0f;
    float cameraDamping_ = 0.02f;
    ZCameraType cameraType_;
    ZCameraMovementStyle movementStyle_;
    ZFrustum frustum_;
    glm::vec3 pitchVelocity_, yawVelocity_;
    glm::quat pitch_, yaw_;

    double currentDeltaTime_ = 0.0;
    double lastDeltaTime_ = 0.0;

    glm::mat4 view_{ 1.f };
    glm::mat4 projection_{ 1.f };
    glm::mat4 viewProjection_{ 1.f };
    glm::mat4 inverseViewProjection_{ 1.f };
    glm::mat4 previousViewProjection_{ 1.f };

    std::shared_ptr<ZUniformBuffer> uniformBuffer_;
    std::shared_ptr<ZRenderStateGroup> renderState_;

    void UpdateCameraFrustum();
    glm::mat4 GenerateProjectionMatrix();
    glm::mat4 GenerateViewMatrix();

    void HandleMove(const std::shared_ptr<ZMoveEvent>& event);
    void HandleLook(const std::shared_ptr<ZLookEvent>& event);

};
