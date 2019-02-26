//
//  ZPhysicsComponent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "btBulletDynamicsCommon.h"
#include "ZCommon.hpp"
#include "ZComponent.hpp"

// Forward Declarations
struct ZOFNode;

// Class and Data Structure Definitions
class ZPhysicsComponent : public ZComponent {
    using ZColliderCreator = btCollisionShape* (ZPhysicsComponent::*)(std::vector<btScalar> params);
public:

  ZPhysicsComponent();
  ~ZPhysicsComponent() { }

  void Initialize(ZOFNode* root) override;

  void Update() override;

  void AddForce(glm::vec3& force);
  void AddForceAtPoint(glm::vec3& force, glm::vec3& point);
  void AddTorque(glm::vec3& torque);
  bool HasFiniteMass() { }

  void SetVelocity(glm::vec3 velocity) { }
  void SetDamping(float damping) { }
  void SetAngularVelocity(glm::vec3 angularVelocity) { }
  void SetAngularDamping(float damping) { }
  void SetAcceleration(glm::vec3 acceleration) { }
  void SetMass(float mass) { }
  void SetInertiaTensor(glm::mat3 inertiaTensor) { }
  void SetAwake(const bool awake = true);
  void SetCanSleep(const bool canSleep = true);

  glm::vec3 Velocity() { return glm::vec3(0.f); }
  glm::vec3 AngularVelocity() { return glm::vec3(0.f); }
  glm::vec3 Acceleration() { return glm::vec3(0.f); }
  glm::mat3 InertiaTensor() { return glm::mat3(1.f); }
  float Damping() { return 1.f; }
  float AngularDamping() { return 1.f; }
  float Mass() { return 1.f; }

  btCollisionShape* CreateBoxCollider(std::vector<btScalar> params);
  btCollisionShape* CreateSphereCollider(std::vector<btScalar> params);
  btCollisionShape* CreateCapsuleCollider(std::vector<btScalar> params);

protected:

  std::map<ZColliderType, ZColliderCreator> colliderCreators_;

  btRigidBody* body_;

};
