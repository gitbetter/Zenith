//
//  ZPhysics.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZPhysics.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZObjectForceRegistry.hpp"
#include "ZGameObject.hpp"

void ZPhysics::Initialize() {
  if (registry_ == nullptr) {
    registry_ = new ZObjectForceRegistry;
  }
}

void ZPhysics::UpdateForces() {
  registry_->UpdateForces();
}
