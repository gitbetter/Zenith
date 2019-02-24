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
#include "ZCollisionPrimitive.hpp"
#include "ZCollisionComponent.hpp"

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

  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() == 0) continue;

    if (prop->id == "velocity") {
      ZOFNumberList* terminal = dynamic_cast<ZOFNumberList*>(prop->values[0]);
      velocity_ = glm::vec3(terminal->value[0], terminal->value[1], terminal->value[2]);
    } else if (prop->id == "angularVelocity") {
      ZOFNumberList* terminal = dynamic_cast<ZOFNumberList*>(prop->values[0]);
      angularVelocity_ = glm::vec3(terminal->value[0], terminal->value[1], terminal->value[2]);
    } else if (prop->id == "damping") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      damping_ = terminal->value;
    } else if (prop->id == "angularDamping") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      angularDamping_ = terminal->value;
    } else if (prop->id == "mass") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      SetMass(terminal->value);
    } else if (prop->id == "inertiaTensor") {
      ZOFNumberList* terminal = dynamic_cast<ZOFNumberList*>(prop->values[0]);
      glm::mat3 inertiaTensor = glm::make_mat3(&terminal->value[0]);
      SetInertiaTensor(inertiaTensor);
    } else if (prop->id == "simulateGravity") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      if (terminal->value == "Yes") {
        ZGravityForce* gravity = new ZGravityForce(glm::vec3(0.f, -25.f, 0.f));
        ZEngine::Physics()->Registry()->Add(object_, gravity);
      }
    }
  }

  ZCollisionComponent* collisionComp = object_->FindComponent<ZCollisionComponent>();
  if (collisionComp) SetInertiaTensor(collisionComp->CollisionPrimitive());
}

void ZPhysicsComponent::Update() {
  assert(object_ != nullptr);
  if (!isAwake_) return;

  glm::mat3 inverseInertiaWorld_ = glm::mat3(object_->ModelMatrix()) * inverseInertiaTensor_;

  previousAcceleration_ = acceleration_;
  acceleration_ = glm::vec3(0.f);
  acceleration_ += forceAccumulator_ * inverseMass_;
  glm::vec3 angularAcceleration = inverseInertiaWorld_ * torqueAccumulator_;

  velocity_ += acceleration_ * ZEngine::UPDATE_STEP_SIZE;
  angularVelocity_ += angularAcceleration * ZEngine::UPDATE_STEP_SIZE;

  velocity_ *= glm::pow(damping_, ZEngine::UPDATE_STEP_SIZE);
  angularVelocity_ *= glm::pow(angularDamping_, ZEngine::UPDATE_STEP_SIZE);

  object_->SetPosition(object_->Position() + velocity_ * ZEngine::UPDATE_STEP_SIZE);
  object_->SetOrientation(glm::mix(object_->Orientation(), glm::quat(angularVelocity_) * object_->Orientation(), ZEngine::UPDATE_STEP_SIZE));

  ClearForceAccumulator();

  if (canSleep_) {
      float currentMotion = glm::dot(velocity_, velocity_) + glm::dot(angularVelocity_, angularVelocity_);
      float bias = glm::pow(0.5f, ZEngine::UPDATE_STEP_SIZE);
      motion_ = bias * motion_ + (1 - bias) * currentMotion;

      if (motion_ < sleepEpsilon_) SetAwake(false);
      else if (motion_ > 10.f * sleepEpsilon_) motion_ = 10 * sleepEpsilon_;
  }
}

void ZPhysicsComponent::SetInertiaTensor(ZCollisionPrimitive* primitive) {
  if (dynamic_cast<ZCollisionBox*>(primitive)) {
    ZCollisionBox* box = dynamic_cast<ZCollisionBox*>(primitive);
    glm::mat3 inertiaTensor(0.f);
    inertiaTensor[0][0] = 0.33f * Mass() * (box->halfSize.y * box->halfSize.y + box->halfSize.z * box->halfSize.z);
    inertiaTensor[1][1] = 0.33f * Mass() * (box->halfSize.x * box->halfSize.x + box->halfSize.z * box->halfSize.z);
    inertiaTensor[2][2] = 0.33f * Mass() * (box->halfSize.x * box->halfSize.x + box->halfSize.y * box->halfSize.y);
    SetInertiaTensor(inertiaTensor);
  }
}

void ZPhysicsComponent::AddForce(glm::vec3& force) {
  forceAccumulator_ += force;
  SetAwake();
}

void ZPhysicsComponent::AddForceAtPoint(glm::vec3& force, glm::vec3& point) {
  glm::vec3 pt = point;
  pt -= object_->Position();

  forceAccumulator_ += force;
  torqueAccumulator_ += glm::cross(pt, force);

  SetAwake();
}

void ZPhysicsComponent::AddForceAtBodyPoint(glm::vec3& force, glm::vec3& point) {
  glm::vec3 worldPoint = glm::vec3(object_->ModelMatrix() * glm::vec4(point, 1.f));
  AddForceAtPoint(force, worldPoint);
}

void ZPhysicsComponent::AddTorque(glm::vec3& torque) {
  torqueAccumulator_ += torque;
  SetAwake();
}

void ZPhysicsComponent::ClearForceAccumulator() {
  forceAccumulator_ = glm::vec3(0.f);
}

void ZPhysicsComponent::SetAwake(const bool awake) {
  isAwake_ = awake;
  if (isAwake_) {
    motion_ = sleepEpsilon_ * 2.f;
  } else {
    velocity_ = glm::vec3(0.f);
    angularVelocity_ = glm::vec3(0.f);
  }
}

void ZPhysicsComponent::SetCanSleep(const bool canSleep) {
  canSleep_ = canSleep;
  if (!canSleep_ && !isAwake_) SetAwake();
}
