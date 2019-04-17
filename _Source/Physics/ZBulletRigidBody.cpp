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

ZBulletRigidBody::ZBulletRigidBody(std::shared_ptr<ZCollider> collider, float mass, glm::vec3 origin, glm::vec3 scale) {
    btCollisionShape* coll = static_cast<btCollisionShape*>(collider->Get());
    
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(origin[0], origin[1], origin[2]));
    
    coll->setLocalScaling(btVector3(scale[0], scale[1], scale[2]));
    
    btVector3 localInertia(0, 0, 0);
    if (mass > 0.f) coll->calculateLocalInertia(mass, localInertia);
    
    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, coll, localInertia);
    btRigidBody* bodyPtr = new btRigidBody(rbInfo);
    ptr_ = bodyPtr;
}

void ZBulletRigidBody::Initialize() {

}

float ZBulletRigidBody::InverseMass() {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    return body->getInvMass();
}

glm::mat4 ZBulletRigidBody::TransformMatrix() {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    btTransform transform; ATTRIBUTE_ALIGNED16(glm::mat4) modelMatrix;
    if (body->getMotionState()) {
      body->getMotionState()->getWorldTransform(transform);
    }
    transform.getOpenGLMatrix(glm::value_ptr(modelMatrix));
    return modelMatrix;
}

glm::vec3 ZBulletRigidBody::Velocity() {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    btVector3 vel = body->getLinearVelocity();
    return glm::vec3(vel.x(), vel.y(), vel.z());
}

glm::vec3 ZBulletRigidBody::AngularVelocity() {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    btVector3 vel = body->getAngularVelocity();
    return glm::vec3(vel.x(), vel.y(), vel.z());
}

void ZBulletRigidBody::ApplyForce(glm::vec3& force) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    body->activate();
    body->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void ZBulletRigidBody::ApplyForceAtPoint(glm::vec3& force, glm::vec3& point) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    body->activate();
    body->applyForce(btVector3(force.x, force.y, force.z), btVector3(point.x, point.y, point.z));
}

void ZBulletRigidBody::ApplyTorque(glm::vec3& torque) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    body->activate();
    body->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

void ZBulletRigidBody::SetGravity(glm::vec3& gravity) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    body->setGravity(btVector3(gravity[0], gravity[1], gravity[2]));
}

void ZBulletRigidBody::SetLinearDamping(float damping) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    body->setDamping(damping, body->getAngularDamping());
}

void ZBulletRigidBody::SetAngularDamping(float damping) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    body->setDamping(body->getLinearDamping(), damping);
}

void ZBulletRigidBody::SetRestitution(float restitution) {
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    body->setRestitution(restitution);
}
