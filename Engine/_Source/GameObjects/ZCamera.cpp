/*
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZCamera.cpp

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

#include "ZCamera.hpp"
#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZMoveEvent.hpp"
#include "ZLookEvent.hpp"
#include "ZOFTree.hpp"
#include "ZDomain.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"

ZCamera::ZCamera(ZCameraType type)
    : ZGameObject(), cameraType(type)
{
    name = "Camera_" + std::to_string(idGenerator_.Next());
    zoom = cameraType == ZCameraType::Orthographic ? 180.f : 45.f;
    movementStyle = ZCameraMovementStyle::Normal;
    pitch = glm::quat(0.f, glm::vec3(1.f, 0.f, 0.f));
    yaw = glm::quat(0.f, glm::vec3(0.f, 1.f, 0.f));
    pitchVelocity = glm::vec3(0.f);
    yawVelocity = glm::vec3(0.f);
}

void ZCamera::OnCreate()
{
    ZServices::EventAgent()->Subscribe(this, &ZCamera::HandleMove);
    ZServices::EventAgent()->Subscribe(this, &ZCamera::HandleLook);

    DisableMovement();
    DisableLook();

    auto sceneSP = scene.lock();
    if (sceneSP == nullptr)
    {
        return;
    }

    frustum = ZFrustum(zoom, sceneSP->Domain()->Aspect(), nearClippingPlane, farClippingPlane);

    uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Camera, sizeof(ZCameraUniforms));
}

void ZCamera::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (dataNode == nullptr)
    {
        LOG("Could not initalize ZCamera", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("primary") != props.end() && props["primary"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["primary"]->Value<ZOFString>(0);
        isPrimary = prop->value == "Yes";
    }

    if (props.find("speed") != props.end() && props["speed"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["speed"]->Value<ZOFNumber>(0);
        movementSpeed = prop->value;
    }

    if (props.find("sensitivity") != props.end() && props["sensitivity"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["sensitivity"]->Value<ZOFNumber>(0);
        lookSensitivity = prop->value * 0.1f;
    }

    if (props.find("style") != props.end() && props["style"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["style"]->Value<ZOFString>(0);
        cameraType = prop->value == "Orthographic" ? ZCameraType::Orthographic : ZCameraType::Perspective;
        zoom = cameraType == ZCameraType::Orthographic ? 180.f : 45.f;
    }

    if (props.find("zoom") != props.end() && props["zoom"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["zoom"]->Value<ZOFNumber>(0);
        zoom = prop->value;
    }

    if (props.find("zoomSpeed") != props.end() && props["zoomSpeed"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["zoomSpeed"]->Value<ZOFNumber>(0);
        zoomSpeed = prop->value;
    }

    if (props.find("nearPlane") != props.end() && props["nearPlane"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["nearPlane"]->Value<ZOFNumber>(0);
        nearClippingPlane = prop->value;
    }

    if (props.find("farPlane") != props.end() && props["farPlane"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["farPlane"]->Value<ZOFNumber>(0);
        farClippingPlane = prop->value;
    }

    if (props.find("movementStyle") != props.end() && props["movementStyle"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["movementStyle"]->Value<ZOFString>(0);
        movementStyle = prop->value == "Follow" ? ZCameraMovementStyle::Follow : ZCameraMovementStyle::Normal;
    }

    if (props.find("damping") != props.end() && props["damping"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["damping"]->Value<ZOFNumber>(0);
        cameraDamping = prop->value;
    }
}

void ZCamera::OnUpdate(double deltaTime)
{
    lastDeltaTime_ = currentDeltaTime_;
    currentDeltaTime_ = deltaTime;

    if (isMoving)
    {
        UpdateCameraFrustum();
    }

    previousViewProjection = viewProjection;
    view = GenerateViewMatrix();
    projection = GenerateProjectionMatrix();
    viewProjection = projection * view;
    inverseViewProjection = glm::inverse(viewProjection);
    auto viewPosition = ZServices::GameObjectManager()->Position(handle);

    uniformBuffer->Update(offsetof(ZCameraUniforms, V), sizeof(view), glm::value_ptr(view));
    uniformBuffer->Update(offsetof(ZCameraUniforms, P), sizeof(projection), glm::value_ptr(projection));
    uniformBuffer->Update(offsetof(ZCameraUniforms, ViewProjection), sizeof(viewProjection), glm::value_ptr(viewProjection));
    uniformBuffer->Update(offsetof(ZCameraUniforms, PreviousViewProjection), sizeof(previousViewProjection), glm::value_ptr(previousViewProjection));
    uniformBuffer->Update(offsetof(ZCameraUniforms, InverseViewProjection), sizeof(inverseViewProjection), glm::value_ptr(inverseViewProjection));
    uniformBuffer->Update(offsetof(ZCameraUniforms, ViewPosition), sizeof(viewPosition), glm::value_ptr(viewPosition));

    if (auto sceneSP = scene.lock())
    {
        frustum.ratio = sceneSP->Domain()->Aspect();
        if (sceneSP->GameConfig().graphics.drawCameraDebug)
        {
            ZServices::Graphics()->DebugDraw(sceneSP, frustum, glm::vec4(1.f));
        }
    }
}

void ZCamera::OnCloned(ZGameObject* original)
{
    if (ZCamera* originalCamera = dynamic_cast<ZCamera*>(original))
    {
        cameraType = originalCamera->cameraType;
        isPrimary = originalCamera->isPrimary;
        movementSpeed = originalCamera->movementSpeed;
        lookSensitivity = originalCamera->lookSensitivity;
        zoom = originalCamera->zoom;
        zoomSpeed = originalCamera->zoomSpeed;
        nearClippingPlane = originalCamera->nearClippingPlane;
        farClippingPlane = originalCamera->farClippingPlane;
        cameraDamping = originalCamera->cameraDamping;
        movementStyle = originalCamera->movementStyle;
        frustum = originalCamera->frustum;
        movementEnabled_ = originalCamera->movementEnabled_;
        lookEnabled_ = originalCamera->lookEnabled_;
    }
}

void ZCamera::UpdateCameraFrustum()
{
    if (!lookEnabled_)
    {
        return;
    }

    auto sceneSP = scene.lock();
    if (!sceneSP)
    {   
        return;
    }

    if (movementStyle == ZCameraMovementStyle::Follow)
    {
        pitchVelocity *= glm::pow(cameraDamping, (float)currentDeltaTime_);
        yawVelocity *= glm::pow(cameraDamping, (float)currentDeltaTime_);

        float epsilonSquared = EPSILON * EPSILON;
        if (glm::length2(pitchVelocity) <= epsilonSquared &&
            glm::length2(yawVelocity) <= epsilonSquared) {
            isMoving = false;
        }
        else {
            pitch = glm::quat(pitchVelocity * (float)currentDeltaTime_);
            yaw = glm::quat(yawVelocity * (float)currentDeltaTime_);
            ZServices::GameObjectManager()->SetOrientation(handle, glm::normalize(pitch * ZServices::GameObjectManager()->Orientation(handle) * yaw));
        }
    }

    const glm::vec3 position = ZServices::GameObjectManager()->Position(handle);
    const glm::vec3 front = ZServices::GameObjectManager()->Front(handle);
    const glm::vec3 up = ZServices::GameObjectManager()->Up(handle);
    frustum.SetBasis(position, position + front, up);
}

void ZCamera::Move(float z, float x, bool useWorldFront)
{
    const glm::vec3 position = ZServices::GameObjectManager()->Position(handle);
    const glm::vec3 front = ZServices::GameObjectManager()->Front(handle);
    const glm::vec3 right = ZServices::GameObjectManager()->Right(handle);

    float velocity = movementSpeed * (float) currentDeltaTime_;
    glm::vec3 newPos = position + ((useWorldFront ? WORLD_FRONT : front) * z * velocity) + (right * x * -velocity);

    ZServices::GameObjectManager()->SetPosition(handle, newPos);

    if (cameraType == ZCameraType::Orthographic)
    {
        zoom += zoomSpeed * z * velocity;
    }

    isMoving = true;
}

void ZCamera::HandleMove(const std::shared_ptr<ZMoveEvent>& event)
{
    if (!movementEnabled_) return;

    Move(event->Z(), event->X());
}

void ZCamera::Look(float pitchDelta, float yawDelta)
{
    if (movementStyle == ZCameraMovementStyle::Follow)
    {
        pitchVelocity += glm::vec3(glm::radians(-pitchDelta * lookSensitivity), 0.f, 0.f);
        yawVelocity += glm::vec3(0.f, glm::radians(yawDelta * lookSensitivity), 0.f);
    }
    else if (movementStyle == ZCameraMovementStyle::Normal)
    {
        pitch = glm::angleAxis(glm::radians(-pitchDelta * lookSensitivity), glm::vec3(1.f, 0.f, 0.f));
        yaw = glm::angleAxis(glm::radians(yawDelta * lookSensitivity), glm::vec3(0.f, 1.f, 0.f));
    }

    isMoving = true;
}

void ZCamera::HandleLook(const std::shared_ptr<ZLookEvent>& event)
{
    if (!lookEnabled_) return;

    Look(event->Pitch(), event->Yaw());
}

glm::mat4 ZCamera::GenerateProjectionMatrix()
{
    glm::mat4 projectionMatrix(1.f);

    auto sceneSP = scene.lock();
    if (sceneSP == nullptr)
    {
        return projectionMatrix;
    }

    if (cameraType == ZCameraType::Orthographic)
    {
        float zoomInverse_ = 1.f / (2.f * zoom);
        glm::vec2 resolution = sceneSP->Domain()->Resolution();
        float left = -((float)resolution.x * zoomInverse_);
        float right = -left;
        float bottom = -((float)resolution.y * zoomInverse_);
        float top = -bottom;
        projectionMatrix = glm::ortho(left, right, bottom, top, -farClippingPlane, farClippingPlane);
    }
    else
    {
        projectionMatrix = glm::perspective(glm::radians(zoom),
            (float)sceneSP->Domain()->Aspect(),
            nearClippingPlane, farClippingPlane);
    }
    return projectionMatrix;
}

glm::mat4 ZCamera::GenerateViewMatrix()
{
    const glm::vec3 position = ZServices::GameObjectManager()->Position(handle);
    const glm::vec3 front = ZServices::GameObjectManager()->Front(handle);
    const glm::vec3 up = ZServices::GameObjectManager()->Up(handle);
    return glm::lookAt(position, position + front, up);
}