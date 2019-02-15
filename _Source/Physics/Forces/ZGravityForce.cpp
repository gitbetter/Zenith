//
//  ZGravityForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGravityForce.hpp"
#include "ZPhysicsComponent.hpp"

void ZGravityForce::UpdateForce(ZGameObject* object) {
  ZPhysicsComponent* physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  if (!physicsComp->HasFiniteMass()) return;

  physicsComp->AddForce(gravity_ * physicsComp->Mass());
}
