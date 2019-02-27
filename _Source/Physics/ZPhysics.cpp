//
//  ZPhysics.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZPhysics.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZPhysicsDebug.hpp"
#include "ZObjectForceRegistry.hpp"
#include "ZGameObject.hpp"

void ZPhysics::Initialize() {
  if (registry_ == nullptr) {
    registry_ = new ZObjectForceRegistry;
  }

  collisionConfig_ = new btDefaultCollisionConfiguration();
  dispatcher_ = new btCollisionDispatcher(collisionConfig_);
  overlappingPairCache_ = new btDbvtBroadphase();
  solver_ = new btSequentialImpulseConstraintSolver;
  dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher_, overlappingPairCache_, solver_, collisionConfig_);

  debugDrawer_ = new ZPhysicsDebug;
  debugDrawer_->Initialize();
  dynamicsWorld_->setDebugDrawer(debugDrawer_);
  dynamicsWorld_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
}

void ZPhysics::Update() {
  registry_->UpdateForces();
  dynamicsWorld_->stepSimulation(ZEngine::UPDATE_STEP_SIZE, 1, ZEngine::UPDATE_STEP_SIZE);
}

void ZPhysics::AddRigidBody(btRigidBody* body) {
  dynamicsWorld_->addRigidBody(body);
  collisionShapes_.push_back(body->getCollisionShape());
}

void ZPhysics::DebugDraw() {
  dynamicsWorld_->debugDrawWorld();
}
