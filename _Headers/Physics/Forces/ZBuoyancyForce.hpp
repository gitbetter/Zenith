//
//  ZBuoyancyForce.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZForceGenerator.hpp"

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
class ZBuoyancyForce : public ZForceGenerator {
private:
public:
  ZBuoyancyForce(float maxDepth, float volume, float waterHeight, float density = 1000.f);
  ~ZBuoyancyForce() { }

  void UpdateForce(ZGameObject* object) override;

protected:
  float maxDepth_ = 10.f;
  float volume_ = 10.f;
  float waterHeight_ = 20.f;
  float liquidDensity_ = 1000.f;
};
