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

ZPhysics::ZPhysics() {
  colliderCreators_["BoxCollider"] = &ZPhysics::CreateBoxCollider;
}

void ZPhysics::Initialize() {
  if (registry_ == nullptr) {
    registry_ = new ZObjectForceRegistry;
  }

  collisionConfig_ = new btDefaultCollisionConfiguration();
  dispatcher_ = new btCollisionDispatcher(collisionConfiguration);
  overlappingPairCache_ = new btDbvtBroadphase();
  solver_ = new btSequentialImpulseConstraintSolver;
  dynamicsWorld_ = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
}

void ZPhysics::Update() {
  registry_->UpdateForces();
  dynamicsWorld_->stepSimulation(ZEngine::DeltaTime(), ZEngine::MAX_FIXED_UPDATE_ITERATIONS, ZEngine::UPDATE_STEP_SIZE);
}

void ZPhysics::AddRigidBody(btRigidBody* body) {
  dynamicsWorld_->addRigidBody(body);
  collisionShapes_.push_back(body->getCollisionShape());
}

btCollisionShape* ZPhysics::CreateBoxCollider() {
  return new btBoxShape;
}

btCollisionShape* ZPhysics::CreateSphereCollider() {
  return new btSphereShape;
}

btCollisionShape* ZPhysics::CreateCapsulerCollider() {
  return new btCapsuleShape;
}
