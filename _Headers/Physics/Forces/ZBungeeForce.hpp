//
//  ZBungeeForce.hpp
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
class ZBungeeForce : public ZForceGenerator {
private:
public:
  ZBungeeForce(ZGameObject* other, float springConstant, float restLength);
  ~ZBungeeForce() { }

  void UpdateForce(ZGameObject* object) override;

protected:
  ZGameObject* other_ = nullptr;
  float springConstant_ = 1.f;
  float restLength_ = 1.f;
};
