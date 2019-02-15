//
//  ZAnchoredSpringForce.hpp
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
class ZAnchoredSpringForce : public ZForceGenerator {
private:
public:
  ZAnchoredSpringForce(glm::vec3 anchor, float springConstant, float restLength);
  ~ZAnchoredSpringForce() { }

  void UpdateForce(ZGameObject* object) override;

protected:
  glm::vec3 anchor_;
  float springConstant_ = 1.f;
  float restLength_ = 1.f;
};
