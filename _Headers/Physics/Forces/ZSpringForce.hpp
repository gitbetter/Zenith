//
//  ZSpringForce.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZForceGenerator.hpp"

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
class ZSpringForce : public ZForceGenerator {
private:

public:

  ZSpringForce(ZGameObject* other, float springConstant, float restLength);
  ~ZSpringForce() { }

  void UpdateForce(ZGameObject* object) override;

protected:

  ZGameObject* other_ = nullptr;
  float springConstant_ = 1.f;
  float restLength_ = 1.f;
};
