//
//  ZPhysicsComponent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZComponent.hpp"

// Forward Declarations
struct ZOFNode;

// Class and Data Structure Definitions
class ZPhysicsComponent : public ZComponent {
private:

  glm::vec3 velocity_, angularVelocity_, forceAccumulator_, torqueAccumulator_;
  float damping_ = 1.0;
  float angularDamping_ = 1.0;
  float inverseMass_ = 1.0;
  glm::mat3 inverseInertiaTensor_;

public:

  ZPhysicsComponent();
  ~ZPhysicsComponent() { }

  void Initialize(ZOFNode* root) override;

  void Update() override;

  void ClearForceAccumulator();
  void AddForce(glm::vec3 force);
  bool HasFiniteMass() { return inverseMass_ != 0.f; }

  void SetVelocity(glm::vec3 velocity) { velocity_ = velocity; }
  void SetDamping(float damping) { damping_ = damping; }
  void SetAngularVelocity(glm::vec3 angularVelocity) { angularVelocity_ = angularVelocity; }
  void SetAngularDamping(float damping) { angularDamping_ = damping; }
  void SetMass(float mass) { assert(mass > 0.f); inverseMass_ = 1.f / mass; }
  void SetInertiaTensor(glm::mat3 inertiaTensor) { inverseInertiaTensor_ = glm::inverse(inertiaTensor); }

  glm::vec3 Velocity() const { return velocity_; }
  float Damping() const { return damping_; }
  glm::vec3 AngularVelocity() const { return angularVelocity_; }
  float AngularDamping() const { return angularDamping_; }
  float Mass() const { return 1.f / inverseMass_; }
  glm::mat3 InertiaTensor() const { return glm::inverse(inverseInertiaTensor_); }

protected:


};
