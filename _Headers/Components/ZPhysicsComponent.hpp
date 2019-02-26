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
private:

public:

  ZPhysicsComponent();
  ~ZPhysicsComponent() { }

  void Initialize(ZOFNode* root) override;

  void Update() override;

  void AddForce(glm::vec3& force);
  void AddForceAtPoint(glm::vec3& force, glm::vec3& point);
  void AddTorque(glm::vec3& torque);
  bool HasFiniteMass() { return inverseMass_ != 0.f; }

  void SetVelocity(glm::vec3 velocity) { velocity_ = velocity; }
  void SetDamping(float damping) { damping_ = damping; }
  void SetAngularVelocity(glm::vec3 angularVelocity) { angularVelocity_ = angularVelocity; }
  void SetAngularDamping(float damping) { angularDamping_ = damping; }
  void SetAcceleration(glm::vec3 acceleration) { acceleration_ = acceleration; }
  void SetMass(float mass) { assert(mass > 0.f); inverseMass_ = 1.f / mass; }
  void SetInertiaTensor(glm::mat3 inertiaTensor) { inverseInertiaTensor_ = glm::inverse(inertiaTensor); }
  void SetAwake(const bool awake = true);
  void SetCanSleep(const bool canSleep = true);

  glm::vec3 Velocity() const { return velocity_; }
  glm::vec3 AngularVelocity() const { return angularVelocity_; }
  glm::vec3 Acceleration() const { return acceleration_; }
  glm::vec3 PreviousAcceleration() const { return previousAcceleration_; }
  float Damping() const { return damping_; }
  float AngularDamping() const { return angularDamping_; }
  float Mass() const { return 1.f / inverseMass_; s}
  float InverseMass() const { return inverseMass_; }
  glm::mat3 InertiaTensor() const { return glm::inverse(inverseInertiaTensor_); }
  glm::mat3 InverseInertiaTensor() const { return inverseInertiaTensor_; }
  bool Awake() const { return isAwake_; }
  bool CanSleep() const { return canSleep_; }

protected:
  
  btRigidBody* body_;

};
