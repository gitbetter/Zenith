//
//  ZGravityForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZGravityForce.hpp"
#include "ZPhysicsComponent.hpp"

void ZGravityForce::UpdateForce(ZGameObject* object) {
  std::shared_ptr<ZPhysicsComponent> physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  if (!physicsComp->HasFiniteMass()) return;

  physicsComp->SetAcceleration(gravity_);

  glm::vec3 weight = gravity_ * physicsComp->Mass();
  physicsComp->AddForce(weight);
}
