//
//  ZPhysicsComponent.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZGravityForce.hpp"
#include "ZObjectForceRegistry.hpp"
#include "ZPhysics.hpp"
#include "ZOFTree.hpp"

ZPhysicsComponent::ZPhysicsComponent() : ZComponent() {
  colliderCreators_[ZColliderType::Box] = &ZPhysicsComponent::CreateBoxCollider;
  colliderCreators_[ZColliderType::Sphere] = &ZPhysicsComponent::CreateSphereCollider;
  colliderCreators_[ZColliderType::Capsule] = &ZPhysicsComponent::CreateCapsuleCollider;
  id_ = "ZCPhysics_" + ZEngine::IDSequence()->Next();
}

// TODO: These initalize functions can get pretty hectic. Maybe there's a better way...
void ZPhysicsComponent::Initialize(ZOFNode* root) {
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZPhysicsComponent", ZERROR);
    return;
  }

  btScalar mass = -1., damping = 1., angularDamping = 1., restitution = 0.;
  ZColliderType type = ZColliderType::None;
  bool gravity = false;
  std::vector<btScalar> size, origin;

  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() == 0) continue;

    if (prop->id == "type") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      if (terminal->value == "Static") mass = 0.f;
    } else if (prop->id == "mass") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      if (mass == -1.f) mass = terminal->value;
    } else if (prop->id == "damping") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      damping = terminal->value;
      if (prop->values.size() > 1)
        angularDamping = (dynamic_cast<ZOFNumber*>(prop->values[0]))->value;
    } else if (prop->id == "restitution") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      restitution = terminal->value;
    } else if (prop->id == "collider") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      if (terminal->value == "Box") type = ZColliderType::Box;
      if (terminal->value == "Sphere") type = ZColliderType::Sphere;
      if (terminal->value == "Capsule") type = ZColliderType::Capsule;

      if (prop->values.size() > 1) {
        ZOFNumberList* scale = dynamic_cast<ZOFNumberList*>(prop->values[1]);
        std::transform(scale->value.begin(), scale->value.end(), std::back_inserter(size), [](float val) { return btScalar(val); });
      }

      if (prop->values.size() > 2) {
        ZOFNumberList* center = dynamic_cast<ZOFNumberList*>(prop->values[2]);
        std::transform(center->value.begin(), center->value.end(), std::back_inserter(origin), [](float val) { return btScalar(val); });
      }
    } else if (prop->id == "hasGravity") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      gravity = terminal->value == "Yes";
    }
  }

  btCollisionShape* collider;
  if (type != ZColliderType::None) {
    collider = (this->*colliderCreators_[type])(size);
  } else {
    _Z("Could not create the given collider for object " + object_->ID() + ". Creating a default collider instead.", ZWARNING);
    collider = (this->*colliderCreators_[ZColliderType::Box])(size);
  }

  btTransform transform;
  transform.setIdentity();
  if (origin.size() >= 3) transform.setOrigin(btVector3(origin[0], origin[1], origin[2]));
  else transform.setOrigin(btVector3(object_->Position().x, object_->Position().y, object_->Position().z));

  if (size.size() < 3)
    collider->setLocalScaling(btVector3(object_->Scale().x, object_->Scale().y, object_->Scale().z));

  if (mass < 0) mass = 0.0;

  btVector3 localInertia(0, 0, 0);
  if (mass > 0.f) collider->calculateLocalInertia(mass, localInertia);

  btDefaultMotionState* motionState = new btDefaultMotionState(transform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collider, localInertia);
  body_ = new btRigidBody(rbInfo);

  if (gravity) body_->setGravity(btVector3(0.0, -25.0, 0.0));
  body_->setDamping(damping, angularDamping);
  body_->setRestitution(restitution);

  ZEngine::Physics()->AddRigidBody(body_);
}

void ZPhysicsComponent::Update() {
  assert(object_ != nullptr && body_ != nullptr);

  // Don't update static rigid bodies, since the physics
  // cannot affect them
  if (body_->getInvMass() == 0.0) return;

  btTransform transform;
  if (body_->getMotionState()) {
    body_->getMotionState()->getWorldTransform(transform);
  }

  glm::mat4 modelMatrix;
  transform.getOpenGLMatrix(glm::value_ptr(modelMatrix));

  object_->SetModelMatrix(modelMatrix);
}

void ZPhysicsComponent::AddForce(glm::vec3& force) {

}

void ZPhysicsComponent::AddForceAtPoint(glm::vec3& force, glm::vec3& point) {

}

void ZPhysicsComponent::AddTorque(glm::vec3& torque) {

}

void ZPhysicsComponent::SetAwake(const bool awake) {

}

void ZPhysicsComponent::SetCanSleep(const bool canSleep) {

}

btCollisionShape* ZPhysicsComponent::CreateBoxCollider(std::vector<btScalar> params) {
  btVector3 extents(1.0, 1.0, 1.0);
  switch (params.size()) {
    case 1: extents = btVector3(params[0], 1.0, 1.0); break;
    case 2: extents = btVector3(params[0], params[1], 1.0); break;
    case 3: extents = btVector3(params[0], params[1], params[2]); break;
    default: break;
  }

  return new btBoxShape(extents);
}

btCollisionShape* ZPhysicsComponent::CreateSphereCollider(std::vector<btScalar> params) {
  btScalar radius = 1.0;
  switch (params.size()) {
    case 1: radius = params[0]; break;
    default: break;
  }

  return new btSphereShape(radius);
}

btCollisionShape* ZPhysicsComponent::CreateCapsuleCollider(std::vector<btScalar> params) {
  btScalar radius = 1.0, height = 1.0;
  switch (params.size()) {
    case 1: radius = params[0]; break;
    case 2: height = params[1]; break;
    default: break;
  }

  return new btCapsuleShape(radius, height);
}
