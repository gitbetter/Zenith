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
  glm::vec3 velocity_, acceleration_, forceAccumulator_;
  float damping_ = 1.0;
  float inverseMass_ = 1.0;

public:
  ZPhysicsComponent() : velocity_(glm::vec3(0.f)), acceleration_(glm::vec3(0.f)), forceAccumulator_(glm::vec3(0.f)) { }
  ~ZPhysicsComponent() { }

  void Integrate();
  void ClearForceAccumulator();
  void AddForce(glm::vec3 force);
  bool HasFiniteMass() { return inverseMass_ != 0.f; }

  void SetVelocity(glm::vec3 velocity) { velocity_ = velocity; }
  void SetAcceleration(glm::vec3 acceleration) { acceleration_ = acceleration; }
  void SetDamping(float damping) { damping_ = damping; }
  void SetMass(float mass) { assert(mass > 0.f); inverseMass_ = 1.f / mass; }

  glm::vec3 Velocity() { return velocity_; }
  glm::vec3 Acceleration() { return acceleration_; }
  float Damping() { return damping_; }
  float Mass() { return 1.f / inverseMass_; }

protected:

};
