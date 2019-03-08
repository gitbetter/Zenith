//
//  ZStiffSpringForce.hpp
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
class ZStiffSpringForce : public ZForceGenerator {
private:

public:

  ZStiffSpringForce(glm::vec3 anchor, float springConstant, float damping);
  ~ZStiffSpringForce() { }

  void UpdateForce(ZGameObject* object) override;

protected:

  glm::vec3 anchor_;
  float springConstant_ = 1.f;
  float damping_ = 0.1f;
};
