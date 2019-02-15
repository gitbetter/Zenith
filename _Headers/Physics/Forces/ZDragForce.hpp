//
//  ZDragForce.hpp
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
class ZDragForce : public ZForceGenerator {
private:
public:
  ZDragForce(float k1, float k2) { k1_ = k1; k2_ = k2; }
  ~ZDragForce() { }

  void UpdateForce(ZGameObject* object) override;

protected:
  float k1_, k2_;
};
