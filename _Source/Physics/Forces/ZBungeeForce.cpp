//
//  ZBungeeForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZBungeeForce.hpp"
#include "ZPhysicsComponent.hpp"

ZBungeeForce::ZBungeeForce(ZGameObject* other, float springConstant, float restLength)
: other_(other), springConstant_(springConstant), restLength_(restLength) { }

void ZBungeeForce::UpdateForce(ZGameObject* object) {
  std::shared_ptr<ZPhysicsComponent> physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  glm::vec3 force = object->Position();
  force -= other_->Position();

  float magnitude = glm::length(force);
  if (magnitude <= restLength_) return;

  magnitude = springConstant_ * (restLength_ - magnitude);

  force = glm::normalize(force);
  force *= -magnitude;
  physicsComp->AddForce(force);
}
