//
//  ZParticle.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 13/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZParticle.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZPhysicsComponent.hpp"

void ZParticle::Initialize() {
  // TODO: Uncomment once approriate model and shader is chosen for
  // displaying a particle
  // if (FindComponent<ZGraphicsComponent>() == nullptr) {
  //   AddComponent(new ZGraphicsComponent(the-model, the-shader));
  // }

  if (FindComponent<ZPhysicsComponent>() == nullptr) {
    AddComponent(std::shared_ptr<ZPhysicsComponent>(new ZPhysicsComponent));
  }
}

void ZParticle::Update() {
  physicsComponent_->Update();
  age_ -= ZEngine::UPDATE_STEP_SIZE;
}

void ZParticle::Render(float frameMix, unsigned char renderOp) {
  // TODO: Render a particle as a quad that rotates with the camera
}
