//
//  ZAnchoredSpringForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZAnchoredSpringForce.hpp"

#include "ZPhysicsComponent.hpp"

ZAnchoredSpringForce::ZAnchoredSpringForce(glm::vec3 anchor, float springConstant, float restLength)
: anchor_(anchor), springConstant_(springConstant), restLength_(restLength) { }

void ZAnchoredSpringForce::UpdateForce(ZGameObject* object) {
  std::shared_ptr<ZPhysicsComponent> physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  glm::vec3 force = object->Position();
  force -= anchor_;

  float magnitude = glm::length(force);
  magnitude = glm::abs(magnitude - restLength_);
  magnitude *= springConstant_;

  force = glm::normalize(force);
  force *= -magnitude;
  physicsComp->AddForce(force);
}
