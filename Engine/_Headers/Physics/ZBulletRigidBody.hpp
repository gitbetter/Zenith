/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZBulletRigidBody.hpp

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
#include "ZRigidBody.hpp"
#include "ZCollider.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZBulletRigidBody : public ZRigidBody {

public:

    ZBulletRigidBody() { colliderOffset_ = glm::vec3(0.f); type_ = ZPhysicsBodyType::Dynamic; }
    ZBulletRigidBody(void* bodyPtr) : ZBulletRigidBody() { ptr_ = bodyPtr; }
    ZBulletRigidBody(ZPhysicsBodyType type, std::shared_ptr<ZCollider> collider, float mass, glm::vec3 origin, glm::vec3 scale, glm::quat rotation);
	~ZBulletRigidBody() { }

	void Initialize() override;

    float InverseMass() override;
    glm::mat4 TransformMatrix() override;
    glm::vec3 Velocity() override;
    glm::vec3 AngularVelocity() override;
    void DisableContactResponse() override;
    
    void SetGravity(glm::vec3& gravity) override;
    void SetLinearDamping(float damping) override;
    void SetAngularDamping(float damping) override;
    void SetRestitution(float restitution) override;
    void SetGameObject(ZGameObject* gameObject) override;    
    
    void ApplyForce(glm::vec3& force) override;
    void ApplyForceAtPoint(glm::vec3& force, glm::vec3& point) override;
    void ApplyTorque(glm::vec3& torque) override;

};
