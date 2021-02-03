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

#include "ZServices.hpp"
#include "ZCamera.hpp"
#include "ZScene.hpp"
#include "ZMoveEvent.hpp"
#include "ZLookEvent.hpp"
#include "ZOFTree.hpp"
#include "ZDomain.hpp"

ZCamera::ZCamera(const glm::vec3& position, const glm::quat& orientation, ZCameraType type)
    : ZGameObject(position, orientation)
{
    cameraType_ = type;
    zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
    movementStyle_ = ZCameraMovementStyle::Normal;
    pitch_ = glm::quat(0.f, glm::vec3(1.f, 0.f, 0.f));
    yaw_ = glm::quat(0.f, glm::vec3(0.f, 1.f, 0.f));
    pitchVelocity_ = glm::vec3(0.f);
    yawVelocity_ = glm::vec3(0.f);
    frustum_ = ZFrustum(zoom_, 1.f, nearClippingPlane_, farClippingPlane_);
    id_ = "ZCAM_" + idGenerator_.Next();
}

void ZCamera::Initialize()
{
    ZServices::EventAgent()->Subscribe(this, &ZCamera::HandleMove);
    ZServices::EventAgent()->Subscribe(this, &ZCamera::HandleLook);

    DisableDefaultMovement();
    DisableDefaultLook();

    ZGameObject::Initialize();
}

void ZCamera::Initialize(std::shared_ptr<ZOFNode> root)
{
    ZGameObject::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZCamera", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("primary") != props.end() && props["primary"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["primary"]->Value<ZOFString>(0);
        isPrimary_ = prop->value == "Yes";
    }

    if (props.find("speed") != props.end() && props["speed"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["speed"]->Value<ZOFNumber>(0);
        movementSpeed_ = prop->value;
    }

    if (props.find("sensitivity") != props.end() && props["sensitivity"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["sensitivity"]->Value<ZOFNumber>(0);
        lookSensitivity_ = prop->value;
    }

    if (props.find("type") != props.end() && props["type"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["type"]->Value<ZOFString>(0);
        cameraType_ = prop->value == "Orthographic" ? ZCameraType::Orthographic : ZCameraType::Perspective;
        zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
    }

    if (props.find("zoom") != props.end() && props["zoom"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["zoom"]->Value<ZOFNumber>(0);
        zoom_ = prop->value;
    }

    if (props.find("zoomSpeed") != props.end() && props["zoomSpeed"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["zoomSpeed"]->Value<ZOFNumber>(0);
        zoomSpeed_ = prop->value;
    }

    if (props.find("nearPlane") != props.end() && props["nearPlane"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["nearPlane"]->Value<ZOFNumber>(0);
        nearClippingPlane_ = prop->value;
    }

    if (props.find("farPlane") != props.end() && props["farPlane"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["farPlane"]->Value<ZOFNumber>(0);
        farClippingPlane_ = prop->value;
    }

    if (props.find("movementStyle") != props.end() && props["movementStyle"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["movementStyle"]->Value<ZOFString>(0);
        movementStyle_ = prop->value == "Follow" ? ZCameraMovementStyle::Follow : ZCameraMovementStyle::Normal;
    }

    if (props.find("damping") != props.end() && props["damping"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["damping"]->Value<ZOFNumber>(0);
        cameraDamping_ = prop->value;
    }

    Initialize();
}

void ZCamera::Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp)
{
    lastDeltaTime_ = currentDeltaTime_;
    currentDeltaTime_ = deltaTime;
    frameMix_ = glm::clamp(
        glm::abs(currentDeltaTime_ - lastDeltaTime_),
        0.0,
        1.0
    );

    UpdateCameraOrientation();

    if (auto scene = Scene()) {
        frustum_.ratio = scene->Domain()->Aspect();
        if (scene->GameConfig().graphics.drawCameraDebug)
        {
            ZServices::Graphics()->DebugDraw(scene, Frustum(), glm::vec4(1.f));
        }
    }
}

std::shared_ptr<ZGameObject> ZCamera::Clone()
{
    std::shared_ptr<ZCamera> clone = ZCamera::Create();
    clone->cameraType_ = cameraType_;
    clone->isPrimary_ = isPrimary_;
    clone->movementSpeed_ = movementSpeed_;
    clone->lookSensitivity_ = lookSensitivity_;
    clone->zoom_ = zoom_;
    clone->zoomSpeed_ = zoomSpeed_;
    clone->nearClippingPlane_ = nearClippingPlane_;
    clone->farClippingPlane_ = farClippingPlane_;
    clone->cameraDamping_ = cameraDamping_;
    clone->movementStyle_ = movementStyle_;
    clone->frustum_ = frustum_;
    clone->movementEnabled_ = movementEnabled_;
    clone->lookEnabled_ = lookEnabled_;
    return clone;
}

bool ZCamera::IsVisible()
{
    if (auto scene = Scene()) {
        std::shared_ptr<ZCamera> activeCamera = scene->ActiveCamera();
        return activeCamera->Frustum().Contains(Frustum()) || ZGameObject::IsVisible();
    }
    return false;
}

void ZCamera::CleanUp()
{
    ZGameObject::CleanUp();

    ZServices::EventAgent()->Unsubscribe(this, &ZCamera::HandleMove);
    ZServices::EventAgent()->Unsubscribe(this, &ZCamera::HandleLook);
}

void ZCamera::UpdateCameraOrientation()
{
    if (!lookEnabled_) return;

    if (movementStyle_ == ZCameraMovementStyle::Follow)
    {
        pitchVelocity_ *= glm::pow(cameraDamping_, (float)currentDeltaTime_);
        yawVelocity_ *= glm::pow(cameraDamping_, (float)currentDeltaTime_);
        pitch_ = glm::quat(pitchVelocity_ * (float)currentDeltaTime_);
        yaw_ = glm::quat(yawVelocity_ * (float)currentDeltaTime_);
        SetOrientation(glm::normalize(pitch_ * Orientation() * yaw_));
    }
}

void ZCamera::Move(float z, float x, bool useWorldFront)
{
    float velocity = movementSpeed_ * (float) currentDeltaTime_;
    glm::vec3 newPos = Position() + ((useWorldFront ? WORLD_FRONT : Front()) * z * velocity) + (Right() * x * -velocity);
    SetPosition(newPos);

    if (cameraType_ == ZCameraType::Orthographic)
    {
        zoom_ += zoomSpeed_ * z * velocity;
    }
    frustum_.Recalculate(Position(), Position() + Front(), Up());
}

void ZCamera::HandleMove(const std::shared_ptr<ZMoveEvent>& event)
{
    if (!movementEnabled_) return;

    Move(event->Z(), event->X());
}

void ZCamera::Look(float pitch, float yaw)
{
    if (movementStyle_ == ZCameraMovementStyle::Follow)
    {
        pitchVelocity_ += glm::vec3(glm::radians(-pitch * lookSensitivity_), 0.f, 0.f);
        yawVelocity_ += glm::vec3(0.f, glm::radians(yaw * lookSensitivity_), 0.f);
    }
    else if (movementStyle_ == ZCameraMovementStyle::Normal)
    {
        pitch_ = glm::angleAxis(glm::radians(-pitch * lookSensitivity_), glm::vec3(1.f, 0.f, 0.f));
        yaw_ = glm::angleAxis(glm::radians(yaw * lookSensitivity_), glm::vec3(0.f, 1.f, 0.f));
        SetOrientation(glm::normalize(pitch_ * Orientation() * yaw_));
    }
    frustum_.Recalculate(Position(), Position() + Front(), Up());
}

void ZCamera::HandleLook(const std::shared_ptr<ZLookEvent>& event)
{
    if (!lookEnabled_) return;

    Look(event->Pitch(), event->Yaw());
}

glm::mat4 ZCamera::ProjectionMatrix()
{
    glm::mat4 projectionMatrix(1.f);

    auto scene = Scene();
    if (!scene) return projectionMatrix;

    if (cameraType_ == ZCameraType::Orthographic)
    {
        float zoomInverse_ = 1.f / (2.f * zoom_);
        glm::vec2 resolution = scene->Domain()->Resolution();
        float left = -((float)resolution.x * zoomInverse_);
        float right = -left;
        float bottom = -((float)resolution.y * zoomInverse_);
        float top = -bottom;
        projectionMatrix = glm::ortho(left, right, bottom, top, -farClippingPlane_, farClippingPlane_);
    }
    else
    {
        projectionMatrix = glm::perspective(glm::radians(zoom_),
            (float)scene->Domain()->Aspect(),
            nearClippingPlane_, farClippingPlane_);
    }
    return projectionMatrix;
}

glm::mat4 ZCamera::ViewMatrix()
{
    //glm::vec3 interpolatedFront = PreviousFront() * (1.f - frameMix_) + Front() * frameMix_;
    //glm::vec3 interpolatedUp = PreviousUp() * (1.f - frameMix_) + Up() * frameMix_;
    return glm::lookAt(Position(), Position() + Front(), Up());
}

DEFINE_OBJECT_CREATORS(ZCamera)