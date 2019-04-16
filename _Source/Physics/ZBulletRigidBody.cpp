/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZBulletRigidBody.cpp

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

#include "ZBulletRigidBody.hpp"
#include "btBulletDynamicsCommon.h"

float ZBulletRigidBody::InverseMass() {
    btRigidBody* body = static_cast<btRigidBody*>(ptr);
    return body->getInvMass();
}

glm::mat4 ZBulletRigidBody::TransformMatrix() {
    btRigidBody* body = static_cast<btRigidBody*>(ptr);
    btTransform transform; glm::mat4 modelMatrix;
    if (body->getMotionState()) {
        body->getMotionState()->getWorldTransform(transform);
    }
    transform.getOpenGLMatrix(glm::value_ptr(modelMatrix));
    return modelMatrix;
}

void ZBulletRigidBody::ApplyForce(glm::vec3& force) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr);
    body->activate();
    body->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void ZBulletRigidBody::ApplyForceAtPoint(glm::vec3& force, glm::vec3& point) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr);
    body->activate();
    body->applyForce(btVector3(force.x, force.y, force.z), btVector3(point.x, point.y, point.z));
}

void ZBulletRigidBody::ApplyTorque(glm::vec3& torque) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr);
    body->activate();
    body->applyTorque(btVector3(torque.x, torque.y, torque.z));
}