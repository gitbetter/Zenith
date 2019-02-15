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

void ZPhysicsComponent::Integrate() {
  assert(object_ != nullptr);

  glm::vec3 acceleration = acceleration_;
  acceleration += forceAccumulator_ * inverseMass_;
  velocity_ += acceleration * ZEngine::UPDATE_STEP_SIZE;
  velocity_ *= glm::pow(damping_, ZEngine::UPDATE_STEP_SIZE);

  object_->SetPosition(object_->Position() + velocity_ * ZEngine::UPDATE_STEP_SIZE);

  ClearForceAccumulator();
}

void ZPhysicsComponent::AddForce(glm::vec3 force) {
  forceAccumulator_ += force;
}

void ZPhysicsComponent::ClearForceAccumulator() {
  forceAccumulator_ = glm::vec3(0.f);
}
