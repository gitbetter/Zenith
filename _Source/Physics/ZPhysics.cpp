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
#include "ZCamera.hpp"

void ZPhysics::Initialize() {
  if (registry_ == nullptr) {
    registry_ = new ZObjectForceRegistry();
  }
}

void ZPhysics::Update(std::vector<ZGameObject*> gameObjects, std::vector<ZCamera*> gameCameras) {
  registry_->UpdateForces();
  for (ZGameObject* object : gameObjects) {
    ZPhysicsComponent* physicsComp = object->FindComponent<ZPhysicsComponent>();
    if (physicsComp != nullptr) physicsComp->Integrate();
  }

  for (ZCamera* camera : gameCameras) {
    ZPhysicsComponent* physicsComp = camera->FindComponent<ZPhysicsComponent>();
    if (physicsComp != nullptr) physicsComp->Integrate();
  }
}
