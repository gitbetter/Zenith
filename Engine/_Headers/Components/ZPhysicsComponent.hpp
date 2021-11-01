/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZPhysicsComponent.hpp

 Created by Adrian Sanchez on 14/02/2019.
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

#include "ZComponent.hpp"

class ZRigidBody;
class ZCollider;

struct ZPhysicsComponent : public ZComponent
{

    RTTR_ENABLE(ZComponent)

public:

    ZPhysicsComponent();
    ~ZPhysicsComponent() = default;

    virtual void OnDeserialize(const std::shared_ptr<struct ZOFObjectNode>& dataNode) override;
    virtual void OnCloned(const ZHComponent& original) override;
    virtual void OnUpdate(double deltaTime) override;
    virtual void OnCleanUp() override;

    void Initialize(ZPhysicsBodyType bodyType, float mass, bool hasGravityInfluence = false);

    void SetLinearDamping(float damping);
    void SetAngularDamping(float damping);
    void SetRestitution(float restitution);
    void SetTransform(const glm::mat4& transform);

    void AddCollider(const std::shared_ptr<ZCollider>& collider);
    void DisableCollisionResponse();
    void AddForce(const glm::vec3& force);
    void AddForceAtPoint(const glm::vec3& force, const glm::vec3& point);
    void AddTorque(const glm::vec3& torque);
    bool HasFiniteMass();

public:

    std::shared_ptr<ZRigidBody> body;

protected:

    bool inUniverse_ = false;

private:

	static ZIDSequence idGenerator_;

};
