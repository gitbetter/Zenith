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
  velocity_ = glm::vec3(0.f);
  angularVelocity_ = glm::vec3(0.f);
  forceAccumulator_ = glm::vec3(0.f);
  torqueAccumulator_ = glm::vec3(0.f);
  acceleration_ = glm::vec3(0.f);
  previousAcceleration_ = glm::vec3(0.f);
  inverseInertiaTensor_ = glm::mat3(1.f);
  id_ = "ZCPhysics_" + ZEngine::IDSequence()->Next();
}

// TODO: These initalize functions can get pretty hectic. Maybe there's a better way...
void ZPhysicsComponent::Initialize(ZOFNode* root) {
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZPhysicsComponent", ZERROR);
    return;
  }

  float mass = -1.f, damping = -1.f, angularDamping = -1.f;
  btCollisionShape* collider;
  bool gravity = false;
  btVector3 size, origin;
  
  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() == 0) continue;

    if (prop->id == "type") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      if (terminal->value == "Static") mass = 0.f;
    } else if (prop->id == "mass") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      if (mass == -1.f) mass = terminal->value;
    } else if (prop->id == "damping") {
      ZOFNumber* terminal0 = dynamic_cast<ZOFNumber*>(prop->values[0]);
      damping = terminal->value;
      if (prop->values.size() > 1)
        angularDamping = (dynamic_cast<ZOFNumber*>(prop->values[0]))->value;
    } else if (prop->id == "collider") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      if (ZEngine::Physics()->ColliderCreators().find(terminal->value) != ZEngine::Physics()->ColliderCreators().end()) {
        collider = ZEngine::Physics()->ColliderCreators()[terminal->value];
      } else {
        _Z("Collider " + terminal->value + " is not available for creation", ZWARNING);
      }
      if (prop->values.size() > 1) {
        ZOFNumberList* scale = dynamic_cast<ZOFString*>(prop->values[1]);
        size = btVector3(scale->value[0], scale->value[1], scale->value[2]);
      }
      if (prop->values.size() > 2) {
        ZOFNumberList* center = dynamic_cast<ZOFString*>(prop->values[2]);
        origin = btVector3(center->value[0], center->value[1], center->value[2]);
      }
    } else if (prop->id == "hasGravity") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      hasGravity = terminal->value == "Yes";
    }
  }

  btTransform transform;
  transform.setIdentity();
  transform.setOrigin(origin);

  btScalar mass(mass);

  btVector3 localInertia(0, 0, 0);
  if (mass > 0.f) collider->calculateLocalInertia(mass, localInertia);

  btDefaultMotionState* motionState = new btDefaultMotionState(groundTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collider, localInertia);
  body_ = new btRigidBody(rbInfo);
  body_.setGravity(btVector3(0.0, -15.0, 0.0));
  body_.setDamping(damping, angularDamping);

  ZEngine::Physics()->AddRigidBody(body_);
}

void ZPhysicsComponent::Update() {
  assert(object_ != nullptr);

  object_->SetPosition(object_->Position() + velocity_ * ZEngine::UPDATE_STEP_SIZE);
  object_->SetOrientation(glm::mix(object_->Orientation(), glm::quat(angularVelocity_) * object_->Orientation(), ZEngine::UPDATE_STEP_SIZE));
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
