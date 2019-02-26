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

// Forward Declarations
class ZObjectForceRegistry;

// Class and Data Structure Definitions
class ZPhysics {
  using ZColliderCreator = btCollisionShape* (ZGOFactory::*)();
public:

  ZPhysics();
  ~ZPhysics() { }

  void Initialize();

  void Update();

  void CleanUp() { }

  ZObjectForceRegistry* Registry() { return registry_; }

  void AddRigidBody(btRigidBody* body);

  std::map<std::string, ZColliderCreator>& ColliderCreators() { return colliderCreators_; }

  btCollisionShape* CreateBoxCollider();
  btCollisionShape* CreateSphereCollider();
  btCollisionShape* CreateCapsulerCollider();

protected:

  std::map<std::string, ZColliderCreator> colliderCreators_;

  ZObjectForceRegistry* registry_ = nullptr;
  btDefaultCollisionConfiguration* collisionConfig_ = nullptr;
  btCollisionDispatcher* dispatcher_ = nullptr;
  btBroadphaseInterface* overlappingPairCache_ = nullptr;
  btSequentialImpulseConstraintSolver* solver_ = nullptr;
  btDiscreteDynamicsWorld* dynamicsWorld_ = nullptr;
  btAlignedObjectArray<btCollisionShape*> collisionShapes_;
};
