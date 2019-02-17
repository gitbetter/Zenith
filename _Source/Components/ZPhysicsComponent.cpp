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

ZPhysicsComponent::ZPhysicsComponent() : ZComponent() {
  velocity_ = glm::vec3(0.f);
  angularVelocity_ = glm::vec3(0.f);
  forceAccumulator_ = glm::vec3(0.f);
  torqueAccumulator_ = glm::vec3(0.f);
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
