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
#include "ZContact.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZIntersectionTests {
public:

  static bool SphereAndHalfSpace(const ZCollisionSphere& sphere, const ZCollisionPlane& plane);
  static bool SphereAndSphere(const ZCollisionSphere& one, const ZCollisionSphere& two);
  static bool BoxAndBox(const ZCollisionBox& one, const ZCollisionBox& two);
  static bool BoxAndHalfSpace(const ZCollisionBox& box, const ZCollisionPlane& plane);
};

struct ZCollisionData {
  ContactPtrList contacts;
  int contactsLeft;
  float friction;
  float restitution;
  float tolerance;

  bool HasMoreContacts() { return contactsLeft > 0; }

  void Reset(unsigned int maxContacts) {
    contacts.clear();
    contactsLeft = maxContacts;
  }

  void AddContact(std::shared_ptr<ZContact> contact) {
    contacts.push_back(contact);
    contactsLeft -= 1;
  }
};

class ZFineCollisionDetector {

private:

  static void FillPointFaceBoxBox(const ZCollisionBox& one, const ZCollisionBox& two,
                                  const glm::vec3& toCenter, ZCollisionData* data, unsigned int best, float pen);

public:

  static unsigned int SphereAndHalfSpace(const ZCollisionSphere& sphere, const ZCollisionPlane& plane, ZCollisionData* data);
  static unsigned int SphereAndTruePlane(const ZCollisionSphere& sphere, const ZCollisionPlane& plane, ZCollisionData* data);
  static unsigned int SphereAndSphere(const ZCollisionSphere& one, const ZCollisionSphere& two, ZCollisionData* data);
  static unsigned int BoxAndHalfSpace(const ZCollisionBox& box, const ZCollisionPlane& plane, ZCollisionData* data);
  static unsigned int BoxAndBox(const ZCollisionBox& one, const ZCollisionBox& two, ZCollisionData* data);
  static unsigned int BoxAndPoint(const ZCollisionBox& box, const glm::vec3& point, ZCollisionData* data);
  static unsigned int BoxAndSphere(const ZCollisionBox& box, const ZCollisionSphere& sphere, ZCollisionData* data);

};
