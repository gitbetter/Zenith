//
//  ZFineCollisionDetector.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 21/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCollisionPrimitive.hpp"

// Forward Declarations
class ZContact;

// Class and Data Structure Definitions
class ZIntersectionTests {
public:

  static bool SphereAndHalfSpace(const ZCollisionSphere& sphere, const ZCollisionPlane& plane);
  static bool SphereAndSphere(const ZCollisionSphere& one, const ZCollisionSphere& two);
  static bool BoxAndBox(const ZCollisionBox& one, const ZCollisionBox& two);
  static bool BoxAndHalfSpace(const ZCollisionBox& box, const ZCollisionPlane& plane);
};

struct ZCollisionData {
  ZContact* firstContact;
  ZContact* contacts;
  int contactsLeft;
  unsigned int contactsCount;
  float friction;
  float restitution;
  float tolerance;

  bool HasMoreContacts() { return contactsLeft > 0; }

  void Reset(unsigned int maxContacts) {
    contactsLeft = maxContacts;
    contactsCount = 0;
    contacts = firstContact;
  }

  void AddContacts(unsigned int count) {
    contactsLeft -= count;
    contactsCount += count;
    contacts += count;
  }
};

class ZFineCollisionDetector {
public:

  static unsigned int SphereAndHalfSpace(const ZCollisionSphere& sphere, const ZCollisionPlane& plane, ZCollisionData* data);
  static unsigned int SphereAndTruePlane(const ZCollisionSphere& sphere, const ZCollisionPlane& plane, ZCollisionData* data);
  static unsigned int SphereAndSphere(const ZCollisionSphere& sphere, const ZCollisionSphere& sphere, ZCollisionData* data);
  static unsigned int BoxAndHalfSpace(const ZCollisionBox& box, const ZCollisionSphere& sphere, ZCollisionData* data);
  static unsigned int BoxAndBox(const ZCollisionBox& one, const ZCollisionBox& two, ZCollisionData* data);
  static unsigned int BoxAndPoint(const ZCollisionBox& box, const glm::vec3& point, ZCollisionData* data);
  static unsigned int BoxAndSphere(const ZCollisionBox& box, const ZCollisionSphere& sphere, ZCollisionData* data);

};
