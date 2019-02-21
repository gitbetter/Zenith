//
//  ZParticleSystem.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGameObject.hpp"
#include <initializer_list>

// Forward Declarations
class ZParticle;

// Class and Data Structure Definitions
struct ZParticleRule {
  float minAge = 0.5f;
  float maxAge = 5.f;
  float minVelocity = 5.f;
  float maxVelocity = 100.f;
  float damping = 0.1f;
};

class ZParticleSystem : public ZGameObject {
private:

  std::vector<ZParticle*> particles_;
  bool isAlive_ = false;

public:

  ZParticleSystem() { }
  ZParticleSystem(std::initializer_list<ZParticleRule> rules);
  ~ZParticleSystem() { }

  void Start() { isAlive_ = true; }
  void Stop() { isAlive_ = false; }

  bool Alive() { return isAlive_; }

  void Update() override;

protected:


};
