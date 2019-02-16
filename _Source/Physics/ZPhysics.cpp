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
    registry_ = new ZObjectForceRegistry();
  }
}

void ZPhysics::Update(std::map<std::string, ZGameObject*>& gameObjects) {
  registry_->UpdateForces();
  for (ZGameObject* object : gameObjects) {
    ZPhysicsComponent* physicsComp = object->FindComponent<ZPhysicsComponent>();
    if (physicsComp != nullptr) physicsComp->Integrate();
  }
}
