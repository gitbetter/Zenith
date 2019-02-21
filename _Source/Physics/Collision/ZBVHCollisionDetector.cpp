//
//  ZBVHCollisionDetector.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 20/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZBVHCollisionDetector.hpp"

ZBoundingSphere::ZBoundingSphere(const glm::vec3 c, float r) {
  center = c;
  radius = r;
}

ZBoundingSphere::ZBoundingSphere(const ZBoundingSphere& one, const ZBoundingSphere& two) {
  glm::vec3 centerOffset = two.center - one.center;
  float distance = glm::distance2(two.center, one.center);
  float radiusDiff = two.radius - one.radius;

  if (radiusDiff * radiusDiff >= distance) {
    if (one.radius > two.radius) {
      center = one.center;
      radius = one.radius;
    } else {
      center = two.center;
      radius = two.radius;
    }
  } else {
    distance = glm::sqrt(distance);
    radius = (distance + one.radius + two.radius) * 0.5f;
    center = one.center;
    if (distance > 0) {
      center += centerOffset * ((radius - one.radius) / distance);
    }
  }
}

bool ZBoundingSphere::Overlaps(const ZBoundingSphere* other) const {
  float distSquared = glm::distance2(center, other->center);
  return distSquared < (radius + other->radius) * (radius + other->radius);
}

float ZBoundingSphere::Growth(const ZBoundingSphere& other) const {
  ZBoundingSphere newSphere(*this, other);
  return newSphere.radius * newSphere.radius - radius * radius;
}
