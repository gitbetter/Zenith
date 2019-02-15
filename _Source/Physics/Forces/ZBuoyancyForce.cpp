//
//  ZBuoyancyForce.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZBuoyancyForce.hpp"
#include "ZPhysicsComponent.hpp"

ZBuoyancyForce::ZBuoyancyForce(float maxDepth, float volume, float waterHeight, float density)
: maxDepth_(maxDepth), volume_(volume), waterHeight_(waterHeight), liquidDensity_(density) { }

void ZBuoyancyForce::UpdateForce(ZGameObject* object) {
  ZPhysicsComponent* physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  float depth = object->Position().y;

  if (depth >= waterHeight_ + maxDepth_) return;
  glm::vec3 force(0.f);

  if (depth <= waterHeight_ - maxDepth_) {
    force.y = liquidDensity_ * volume_;
    physicsComp->AddForce(force);
  }

  force.y = liquidDensity_ * volume_ * (depth - maxDepth_ - waterHeight_) / (2.f * maxDepth_);
  physicsComp->AddForce(force);
}
