//
//  ZGravityForce.hpp
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
class ZGravityForce : public ZForceGenerator {
private:

public:

  ZGravityForce(glm::vec3 gravity) { gravity_ = gravity; }
  ~ZGravityForce() { }

  void UpdateForce(ZGameObject* object) override;
protected:

  glm::vec3 gravity_;
};
