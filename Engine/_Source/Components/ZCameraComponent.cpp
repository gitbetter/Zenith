/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

  ZCameraComponent.cpp

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

#include "ZCameraComponent.hpp"
#include "ZGraphicsDebug.hpp"
#include "ZDomain.hpp"
#include "ZGameObject.hpp"
#include "ZEventAgent.hpp"
#include "ZMoveEvent.hpp"
#include "ZLookEvent.hpp"
#include "ZOFTree.hpp"

ZCameraComponent::ZCameraComponent(ZCameraType type) : ZComponent()
{
    cameraType_ = type;
    zoom_ = cameraType_ == ZCameraType::Orthographic ? 180.f : 45.f;
    movementStyle_ = ZCameraMovementStyle::Normal;
    pitch_ = glm::quat(0.f, glm::vec3(1.f, 0.f, 0.f));
    yaw_ = glm::quat(0.f, glm::vec3(0.f, 1.f, 0.f));
    pitchVelocity_ = glm::vec3(0.f);
    yawVelocity_ = glm::vec3(0.f);
    frustum_ = ZFrustum(zoom_, zenith::Domain()->Aspect(), nearClippingPlane_, farClippingPlane_);
    id_ = "ZCCamera_" + zenith::IDSequence()->Next();
}

void ZCameraComponent::Initialize()
{
    frustum_ = ZFrustum(zoom_, zenith::Domain()->Aspect(), nearClippingPlane_, farClippingPlane_);

    ZEventDelegate moveDelegate = fastdelegate::MakeDelegate(this, &ZCameraComponent::HandleMove);
    ZEventDelegate lookDelegate = fastdelegate::MakeDelegate(this, &ZCameraComponent::HandleLook);
    zenith::EventAgent()->AddListener(moveDelegate, ZMoveEvent::Type);
    zenith::EventAgent()->AddListener(lookDelegate, ZLookEvent::Type);

    DisableDefaultMovement();
    DisableDefaultLook();

    ZComponent::Initialize();
}

// TODO: Initialize functions should come in two flavors
void ZCameraComponent::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        zenith::Log("Could not initalize ZCameraComponent", ZSeverity::Error);
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

void ZCameraComponent::Update(double deltaTime)
{
    currentDeltaTime_ = deltaTime;
    frameMix_ = glm::clamp(
        glm::abs(currentDeltaTime_ - ((double)zenith::UPDATE_STEP_SIZE * (double)zenith::MAX_FIXED_UPDATE_ITERATIONS)),
        0.0,
        1.0
    );
    UpdateCameraOrientation();
    frustum_.Recalculate(object_->Position(), object_->Position() + object_->Front(), object_->Up());
}

std::shared_ptr<ZComponent> ZCameraComponent::Clone()
{
    std::shared_ptr<ZCameraComponent> clone = std::make_shared<ZCameraComponent>(cameraType_);
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

void ZCameraComponent::Render()
{
    glm::vec4 drawColor(1.f, 1.f, 1.f, 1.f);
    zenith::Graphics()->DebugDrawer()->Draw(frustum_, drawColor);
}

void ZCameraComponent::CleanUp()
{
    ZComponent::CleanUp();

    ZEventDelegate moveDelegate = fastdelegate::MakeDelegate(this, &ZCameraComponent::HandleMove);
    ZEventDelegate lookDelegate = fastdelegate::MakeDelegate(this, &ZCameraComponent::HandleLook);
    zenith::EventAgent()->RemoveListener(moveDelegate, ZMoveEvent::Type);
    zenith::EventAgent()->RemoveListener(lookDelegate, ZLookEvent::Type);
}

void ZCameraComponent::UpdateCameraOrientation()
{
    if (!lookEnabled_) return;

    if (movementStyle_ == ZCameraMovementStyle::Follow)
    {
        pitchVelocity_ *= glm::pow(cameraDamping_, (float) currentDeltaTime_);
        yawVelocity_ *= glm::pow(cameraDamping_, (float) currentDeltaTime_);
        pitch_ = glm::quat(pitchVelocity_ * (float) currentDeltaTime_);
        yaw_ = glm::quat(yawVelocity_ * (float) currentDeltaTime_);
        object_->SetOrientation(glm::normalize(pitch_ * object_->Orientation() * yaw_));
    }
}

void ZCameraComponent::Move(float z, float x, bool useWorldFront)
{
    float velocity = movementSpeed_ * (float) currentDeltaTime_;
    glm::vec3 newPos = object_->Position() + ((useWorldFront ? zenith::WORLD_FRONT : object_->Front()) * z * velocity) + (object_->Right() * x * -velocity);
    object_->SetPosition(newPos);

    if (cameraType_ == ZCameraType::Orthographic)
    {
        zoom_ += zoomSpeed_ * z * velocity;
    }
}

void ZCameraComponent::HandleMove(const std::shared_ptr<ZEvent>& event)
{
    if (!movementEnabled_) return;

    std::shared_ptr<ZMoveEvent> moveEvent = std::static_pointer_cast<ZMoveEvent>(event);

    Move(moveEvent->Z(), moveEvent->X());
}

void ZCameraComponent::Look(float pitch, float yaw)
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
        object_->SetOrientation(glm::normalize(pitch_ * object_->Orientation() * yaw_));
    }
}

void ZCameraComponent::HandleLook(const std::shared_ptr<ZEvent>& event)
{
    if (!lookEnabled_) return;

    std::shared_ptr<ZLookEvent> lookEvent = std::static_pointer_cast<ZLookEvent>(event);

    Look(lookEvent->Pitch(), lookEvent->Yaw());
}

glm::mat4 ZCameraComponent::ProjectionMatrix()
{
    const ZDomain* domain = zenith::Domain();
    glm::mat4 projectionMatrix;
    if (cameraType_ == ZCameraType::Orthographic)
    {
        float zoomInverse_ = 1.f / (2.f * zoom_);
        float left = -((float) domain->ResolutionX() * zoomInverse_);
        float right = -left;
        float bottom = -((float) domain->ResolutionY() * zoomInverse_);
        float top = -bottom;
        projectionMatrix = glm::ortho(left, right, bottom, top, -farClippingPlane_, farClippingPlane_);
    }
    else
    {
        projectionMatrix = glm::perspective(glm::radians(zoom_),
            (float) domain->Aspect(),
            nearClippingPlane_, farClippingPlane_);
    }
    return projectionMatrix;
}

glm::mat4 ZCameraComponent::ViewMatrix()
{
    glm::vec3 interpolatedFront = object_->PreviousFront() * (1.f - frameMix_) + object_->Front() * frameMix_;
    glm::vec3 interpolatedUp = object_->PreviousUp() * (1.f - frameMix_) + object_->Up() * frameMix_;
    return glm::lookAt(object_->Position(), object_->Position() + interpolatedFront, interpolatedUp);
}
