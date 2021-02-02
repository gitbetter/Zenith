/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZRigidBody.hpp

    Created by Adrian Sanchez on 16/04/2019.
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

// Forward Declarations
class ZCollider;

// Class and Data Structure Definitions
class ZRigidBody
{

public:

    virtual ~ZRigidBody() {}

    virtual void Initialize() = 0;

    virtual float InverseMass() = 0;
    virtual glm::mat4 TransformMatrix() = 0;
    virtual glm::vec3 Position() = 0;
    virtual glm::quat Rotation() = 0;
    virtual glm::vec3 Scale() = 0;
    virtual glm::vec3 Velocity() = 0;
    virtual glm::vec3 AngularVelocity() = 0;
    virtual ZPhysicsBodyType Type() const { return type_; }

    void* Get() { return ptr_; }

    virtual std::shared_ptr<ZRigidBody> Clone() = 0;

    virtual void ApplyForce(const glm::vec3& force) = 0;
    virtual void ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& point) = 0;
    virtual void ApplyTorque(const glm::vec3& torque) = 0;
    virtual void DisableContactResponse() = 0;

    virtual void SetGravity(const glm::vec3& gravity) = 0;
    virtual void SetLinearDamping(float damping) = 0;
    virtual void SetAngularDamping(float damping) = 0;
    virtual void SetRestitution(float restitution) = 0;
    virtual void SetColliderOffset(const glm::vec3& offset) { colliderOffset_ = offset; }
    virtual void SetGameObject(ZGameObject* gameObject) { gameObject_ = gameObject; }
    virtual void SetPosition(const glm::vec3& position) = 0;
    virtual void SetRotation(const glm::quat& rotation) = 0;
    virtual void SetScale(const glm::vec3& scale) = 0;

protected:

    void* ptr_ = nullptr;
    ZGameObject* gameObject_ = nullptr;
    std::shared_ptr<ZCollider> collider_;
    glm::vec3 colliderOffset_;
    ZPhysicsBodyType type_;

};
