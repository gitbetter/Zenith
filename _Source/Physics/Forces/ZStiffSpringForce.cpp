//
//  ZStiffSpringForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZStiffSpringForce.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZEngine.hpp"

ZStiffSpringForce::ZStiffSpringForce(glm::vec3 anchor, float springConstant, float damping)
: anchor_(anchor), springConstant_(springConstant), damping_(damping) { }

void ZStiffSpringForce::UpdateForce(ZGameObject* object) {
  ZPhysicsComponent* physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  if (!physicsComp->HasFiniteMass()) return;

  glm::vec3 position = object->Position();
  position -= anchor_;

  float gamma = 0.5f * glm::sqrt(4.f * springConstant_ - damping_ * damping_);
  if (gamma == 0.0f) return;

  glm::vec3 c = position * (damping_ / (2.f * gamma)) +
                physicsComp->Velocity() * (1.f / gamma);
  glm::vec3 target = position * glm::cos(gamma * ZEngine::UPDATE_STEP_SIZE) +
                     c * glm::sin(gamma * ZEngine::UPDATE_STEP_SIZE);
  target *= glm::exp(-0.5f * ZEngine::UPDATE_STEP_SIZE * damping_);

  glm::vec3 acceleration = (target - position) * (1.f / ZEngine::UPDATE_STEP_SIZE * ZEngine::UPDATE_STEP_SIZE) -
                           physicsComp->Velocity() * ZEngine::UPDATE_STEP_SIZE;
  physicsComp->AddForce(acceleration * physicsComp->Mass());
}
