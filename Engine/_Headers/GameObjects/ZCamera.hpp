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

#include "ZGameObject.hpp"
#include "ZFrustum.hpp"

struct ZOFNode;
class ZLookEvent;
class ZMoveEvent;
class ZUniformBuffer;
class ZRenderStateGroup;

struct ZCamera : public ZGameObject
{

public:

    ZCamera(ZCameraType type = ZCameraType::Perspective);
    ~ZCamera() = default;

    virtual void OnCreate() override;
    virtual void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) override;
    virtual void OnCloned(ZGameObject* original) override;
    virtual void OnUpdate(double deltaTime) override;

    void Move(float z, float x, bool useWorldFront = false);
    void EnableMovement() { movementEnabled_ = true; }
    void DisableMovement() { movementEnabled_ = false; }

    void Look(float pitch, float yaw);
    void EnableLook() { lookEnabled_ = true; }
    void DisableLook() { lookEnabled_ = false; }

public:

    bool isPrimary = false;
    bool isMoving = true;
    float movementSpeed = 25.f;
    float lookSensitivity = 0.1f;
    float zoom = 45.0f;
    float zoomSpeed = 5.0f;
    float nearClippingPlane = 0.01f;
    float farClippingPlane = 1000.0f;
    float cameraDamping = 0.02f;
    ZCameraType cameraType;
    ZCameraMovementStyle movementStyle;
    ZFrustum frustum;
    glm::vec3 pitchVelocity, yawVelocity;
    glm::quat pitch, yaw;

    glm::mat4 view{ 1.f };
    glm::mat4 projection{ 1.f };
    glm::mat4 viewProjection{ 1.f };
    glm::mat4 inverseViewProjection{ 1.f };
    glm::mat4 previousViewProjection{ 1.f };

private:

    void UpdateCameraFrustum();
    glm::mat4 GenerateProjectionMatrix();
    glm::mat4 GenerateViewMatrix();

    void HandleMove(const std::shared_ptr<ZMoveEvent>& event);
    void HandleLook(const std::shared_ptr<ZLookEvent>& event);

private:

    bool movementEnabled_ = true;
    bool lookEnabled_ = true;
    double currentDeltaTime_ = 0.0;
    double lastDeltaTime_ = 0.0;

    static ZIDSequence idGenerator_;

};
