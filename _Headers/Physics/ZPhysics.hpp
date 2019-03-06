//
//  ZPhysics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "btBulletDynamicsCommon.h"
#include "ZCommon.hpp"
#include "ZPhysicsDebug.hpp"
#include "ZObjectForceRegistry.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZPhysics {

private:

  std::unique_ptr<ZPhysicsDebug> debugDrawer_;
  std::unique_ptr<ZObjectForceRegistry> registry_;
  btDefaultCollisionConfiguration* collisionConfig_ = nullptr;
  btCollisionDispatcher* dispatcher_ = nullptr;
  btBroadphaseInterface* overlappingPairCache_ = nullptr;
  btSequentialImpulseConstraintSolver* solver_ = nullptr;
  btDiscreteDynamicsWorld* dynamicsWorld_ = nullptr;

public:

  ZPhysics() { }
  ~ZPhysics() { }

  void Initialize();

  void Update();

  ZObjectForceRegistry* Registry() { return registry_.get(); }

  void AddRigidBody(std::shared_ptr<btRigidBody> body);

  ZGameObject* Raycast(glm::vec3 start, glm::vec3 direction);

  void DebugDraw();

  void CleanUp() { }

};
