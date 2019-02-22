//
//  ZCollisionPrimitive.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 21/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
class ZIntersectionTests;
class ZCollisionDetector;

// Class and Data Structure Definitions
class ZCollisionPrimitive {
  friend class ZIntersectionTests;
  friend class ZCollisionDetector;

public:

  ZGameObject* body;
  glm::mat4 offset;

  void CalculateInternals();
  glm::vec3 Axis(unsigned int index) const {
    return transform[index];
  }

protected:

  glm::mat4 transform;
};

class ZCollisionSphere : public ZCollisionPrimitive {
public:
  float radius;
};

class ZCollisionPlane : public ZCollisionPrimitive {
public:
  glm::vec3 direction;
  float offset;
};

class ZCollisionBox : public ZCollisionPrimitive {
public:
  glm::vec3 halfSize;
};
