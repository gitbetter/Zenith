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
class ZFineCollisionDetector;

// Class and Data Structure Definitions
class ZCollisionPrimitive {
  friend class ZIntersectionTests;
  friend class ZFineCollisionDetector;

public:
  ZGameObject* body = nullptr;
  glm::mat4 offset = glm::mat4(0.f);

  virtual ~ZCollisionPrimitive() { }

  void CalculateInternals();
  glm::vec3 Axis(unsigned int index) const {
    return transform[index];
  }

  static ZCollisionPrimitive* Create(std::string type, glm::vec3 min, glm::vec3 max);

protected:

  glm::mat4 transform = glm::mat4(0.f);

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
