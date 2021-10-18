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

ZBulletRigidBody::ZBulletRigidBody(ZPhysicsBodyType type, float mass, const glm::vec3& origin, const glm::vec3& scale, const glm::quat& rotation) : ZBulletRigidBody()
{
    btCompoundShape* coll = new btCompoundShape();
    coll->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

    btTransform transform; transform.setIdentity();
    transform.setOrigin(btVector3(origin.x, origin.y, origin.z));
    transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    btVector3 localInertia(0, 0, 0);
    if (mass > 0.f) coll->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, coll, localInertia);
    btRigidBody* bodyPtr = new btRigidBody(rbInfo);

    ptr_ = bodyPtr;
    type_ = type;

    AddCollider(ZCollider::Create(ZColliderType::None, glm::vec3(0.f)));
}

void ZBulletRigidBody::Initialize()
{
    if (type_ == ZPhysicsBodyType::Trigger)
    {
        DisableContactResponse();
    }
}

float ZBulletRigidBody::InverseMass()
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return 0.f;

    return body->getInvMass();
}

glm::mat4 ZBulletRigidBody::TransformMatrix()
{
// TODO: Might be best to cache this transform and update it when necessary
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    btTransform transform; ATTRIBUTE_ALIGNED16(glm::mat4) modelMatrix;
    if (!body) return glm::mat4(1.f);

    if (body->getMotionState())
    {
        body->getMotionState()->getWorldTransform(transform);
    }
    transform.getOpenGLMatrix(glm::value_ptr(modelMatrix));

    return modelMatrix;
}

glm::vec3 ZBulletRigidBody::Position()
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return glm::vec3(0.f);

    const btVector3& pos = body->getCenterOfMassPosition();
    return glm::vec3(pos.x(), pos.y(), pos.z());
}

glm::quat ZBulletRigidBody::Rotation()
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return glm::quat(1.f, 0.f, 0.f, 0.f);

    const btQuaternion& orn = body->getOrientation();
    return glm::quat(orn.w(), orn.x(), orn.y(), orn.z());
}

glm::vec3 ZBulletRigidBody::Scale()
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body || !body->getCollisionShape()) return glm::vec3(0.f);

    btCollisionShape* coll = body->getCollisionShape();
    const btVector3& scale = coll->getLocalScaling();
    return glm::vec3(scale.x(), scale.y(), scale.z());
}

glm::vec3 ZBulletRigidBody::Velocity()
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return glm::vec3(0.f);

    btVector3 vel = body->getLinearVelocity();
    return glm::vec3(vel.x(), vel.y(), vel.z());
}

glm::vec3 ZBulletRigidBody::AngularVelocity()
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return glm::vec3(0.f);

    btVector3 vel = body->getAngularVelocity();
    return glm::vec3(vel.x(), vel.y(), vel.z());
}

void ZBulletRigidBody::DisableContactResponse()
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->setCollisionFlags(body->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
}

void ZBulletRigidBody::AddCollider(const std::shared_ptr<ZCollider> collider)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    glm::vec3 offset = collider->Offset();
    btCompoundShape* coll = static_cast<btCompoundShape*>(body->getCollisionShape());
    btCollisionShape* childColl = static_cast<btCollisionShape*>(collider->Get());
    btTransform t; t.setIdentity(); t.setOrigin(btVector3(offset.x, offset.y, offset.z));
    childColl->setLocalScaling(coll->getLocalScaling());
    coll->addChildShape(t, childColl);
}

std::shared_ptr<ZRigidBody> ZBulletRigidBody::Clone()
{
    glm::vec3 position = Position();
    glm::quat rotation = Rotation();
    glm::vec3 scale = Scale();
    float mass = 1.f / InverseMass();

    std::shared_ptr<ZBulletRigidBody> clone = std::make_shared<ZBulletRigidBody>(type_, mass, position, scale, rotation);

    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    btRigidBody* clonedBody = static_cast<btRigidBody*>(clone->ptr_);
    clonedBody->setCollisionFlags(body->getCollisionFlags());

    return clone;
}

void ZBulletRigidBody::ApplyForce(const glm::vec3& force)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->activate();
    body->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void ZBulletRigidBody::ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& point)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->activate();
    body->applyForce(btVector3(force.x, force.y, force.z), btVector3(point.x, point.y, point.z));
}

void ZBulletRigidBody::ApplyTorque(const glm::vec3& torque)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->activate();
    body->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

void ZBulletRigidBody::SetGravity(const glm::vec3& gravity)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->setGravity(btVector3(gravity[0], gravity[1], gravity[2]));
}

void ZBulletRigidBody::SetLinearDamping(float damping)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->setDamping(static_cast<double>(damping), body->getAngularDamping());
}

void ZBulletRigidBody::SetAngularDamping(float damping)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->setDamping(body->getLinearDamping(), damping);
}

void ZBulletRigidBody::SetRestitution(float restitution)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    body->setRestitution(restitution);
}

void ZBulletRigidBody::SetGameObject(const ZHGameObject& gameObject)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (body == nullptr)
    {
        return;
    }

    gameObject_ = gameObject;
    body->setUserPointer(&gameObject_);
}

void ZBulletRigidBody::SetTransformMatrix(const glm::mat4& matrix)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::decompose(matrix, scale, rotation, translation, glm::vec3(0.f), glm::vec4(0.f));
    
    SetPosition(translation);
    SetRotation(rotation);
    SetScale(scale);
}

void ZBulletRigidBody::SetPosition(const glm::vec3& position)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    btTransform newTransform; newTransform.setIdentity();
    if (body->getMotionState())
    {
        body->getMotionState()->getWorldTransform(newTransform);
    }
    newTransform.setOrigin(btVector3(position.x, position.y, position.z));

    body->setWorldTransform(newTransform);
    body->getMotionState()->setWorldTransform(newTransform);
    body->activate();
}

void ZBulletRigidBody::SetRotation(const glm::quat& rotation)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body) return;

    btTransform newTransform; newTransform.setIdentity();
    if (body->getMotionState())
    {
        body->getMotionState()->getWorldTransform(newTransform);
    }
    newTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

    body->setWorldTransform(newTransform);
    body->getMotionState()->setWorldTransform(newTransform);
    body->activate();
}

void ZBulletRigidBody::SetScale(const glm::vec3& scale)
{
    btRigidBody* body = static_cast<btRigidBody*>(ptr_);
    if (!body || !body->getCollisionShape()) return;

    btCompoundShape* coll = static_cast<btCompoundShape*>(body->getCollisionShape());
    coll->setLocalScaling(btVector3(scale[0], scale[1], scale[2]));
    for (int i = 0; i < coll->getNumChildShapes(); i++) {
        btCollisionShape* childColl = coll->getChildShape(i);
        childColl->setLocalScaling(btVector3(scale[0], scale[1], scale[2]));
    }
}
