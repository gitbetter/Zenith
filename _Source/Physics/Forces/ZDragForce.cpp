//
//  ZDragForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZDragForce.hpp"
#include "ZPhysicsComponent.hpp"

void ZDragForce::UpdateForce(ZGameObject* object) {
  ZPhysicsComponent* physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  glm::vec3 force = physicsComp->Velocity();
  float dragCoefficient = glm::length(force);
  dragCoefficient = k1_ * dragCoefficient + k2_ * dragCoefficient * dragCoefficient;

  force = glm::normalize(force);
  force *= -dragCoefficient;
  physicsComp->AddForce(force);
}
