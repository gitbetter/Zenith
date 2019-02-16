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
// class SomeClass;

// Class and Data Structure Definitions
class ZPhysicsComponent : public ZComponent {
private:
  glm::vec3 velocity_, angularVelocity_, forceAccumulator_, torqueAccumulator_;
  float damping_ = 1.0;
  float angularDamping_ = 1.0;
  float inverseMass_ = 1.0;

public:
  ZPhysicsComponent();
  ~ZPhysicsComponent() { }

  void Integrate();
  void ClearForceAccumulator();
  void AddForce(glm::vec3 force);
  bool HasFiniteMass() { return inverseMass_ != 0.f; }

  void SetVelocity(glm::vec3 velocity) { velocity_ = velocity; }
  void SetDamping(float damping) { damping_ = damping; }
  void SetMass(float mass) { assert(mass > 0.f); inverseMass_ = 1.f / mass; }

  glm::vec3 Velocity() { return velocity_; }
  float Damping() { return damping_; }
  float Mass() { return 1.f / inverseMass_; }

protected:

};
