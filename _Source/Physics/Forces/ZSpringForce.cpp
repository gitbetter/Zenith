//
//  ZSpringForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZSpringForce.hpp"
#include "ZPhysicsComponent.hpp"

ZSpringForce::ZSpringForce(ZGameObject* other, float springConstant, float restLength)
: other_(other), springConstant_(springConstant), restLength_(restLength) { }

void ZSpringForce::UpdateForce(ZGameObject* object) {
  std::shared_ptr<ZPhysicsComponent> physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  glm::vec3 force = object->Position();
  force -= other_->Position();

  float magnitude = glm::length(force);
  magnitude = glm::abs(magnitude - restLength_);
  magnitude *= springConstant_;

  force = glm::normalize(force);
  force *= -magnitude;
  physicsComp->AddForce(force);
}
