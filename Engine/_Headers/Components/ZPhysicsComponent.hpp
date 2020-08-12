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

// Includes
#include "ZComponent.hpp"

// Forward Declarations
struct ZOFNode;
class ZRigidBody;

// Class and Data Structure Definitions
class ZPhysicsComponent : public ZComponent
{

public:

    ZPhysicsComponent();
    ~ZPhysicsComponent() {}

    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override;
    void Initialize(std::string bodyType, std::string colliderType, float mass, glm::vec3 position, glm::vec3 scale, glm::quat rotation);

    std::shared_ptr<ZComponent> Clone() override;

    void Update() override;

    void CleanUp() override;

    void DisableCollisionResponse();
    void AddForce(glm::vec3& force);
    void AddForceAtPoint(glm::vec3& force, glm::vec3& point);
    void AddTorque(glm::vec3& torque);
    bool HasFiniteMass();

    std::shared_ptr<ZRigidBody> RigidBody() { return body_; }

protected:

    std::shared_ptr<ZRigidBody> body_;

};
