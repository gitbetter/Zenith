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
#include "ZOFTree.hpp"

ZPhysicsComponent::ZPhysicsComponent() : ZComponent() {
  velocity_ = glm::vec3(0.f);
  angularVelocity_ = glm::vec3(0.f);
  forceAccumulator_ = glm::vec3(0.f);
  torqueAccumulator_ = glm::vec3(0.f);
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
    }
  }
}

void ZPhysicsComponent::Integrate() {
  assert(object_ != nullptr);

  glm::mat3 inverseInertiaWorld_ = glm::mat3_cast(object_->Orientation()) * inverseInertiaTensor_;

  glm::vec3 acceleration(0.f);
  acceleration += forceAccumulator_ * inverseMass_;
  glm::vec3 angularAcceleration = inverseInertiaTensor_ * torqueAccumulator_;

  velocity_ += acceleration * ZEngine::UPDATE_STEP_SIZE;
  angularVelocity_ += angularAcceleration * ZEngine::UPDATE_STEP_SIZE;

  velocity_ *= glm::pow(damping_, ZEngine::UPDATE_STEP_SIZE);
  angularVelocity_ *= glm::pow(angularDamping_, ZEngine::UPDATE_STEP_SIZE);

  object_->SetPosition(object_->Position() + velocity_ * ZEngine::UPDATE_STEP_SIZE);
  object_->SetOrientation(glm::mix(object_->Orientation(), glm::quat(angularVelocity_) * object_->Orientation(), ZEngine::UPDATE_STEP_SIZE));

  ClearForceAccumulator();
}

void ZPhysicsComponent::AddForce(glm::vec3 force) {
  forceAccumulator_ += force;
}

void ZPhysicsComponent::ClearForceAccumulator() {
  forceAccumulator_ = glm::vec3(0.f);
}
