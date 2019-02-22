//
//  ZFineCollisionDetector.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 21/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZFineCollisionDetector.hpp"

static inline float TransformToAxis(const ZCollisionBox& box, const glm::vec3& axis) {
  return box.halfSize.x * glm::abs(glm::dot(axis, box.Axis(0))) +
         box.halfSize.y * glm::abs(glm::dot(axis, box.Axis(1))) +
         box.halfSize.z * glm::abs(glm::dot(axis, box.Axis(2)));
}

static inline bool OverlapOnAxis(const ZCollisionBox& one, const ZCollisionBox& two,
                                 const glm::vec3& axis, glm::vec3& toCenter) {
  float oneProjection = TransformToAxis(one, axis);
  float twoProjection = TransformToAxis(two, axis);
  float distance = glm::abs(glm::dot(toCenter, axis));
  return distance < oneProjection + twoProjection;
}

static inline float PenetrationOnAxis(const ZCollisionBox& one, const ZCollisionBox& two,
                                      const glm::vec3& axis, glm::vec3& toCenter) {
  float oneProjection = TransformToAxis(one, axis);
  float twoProjection = TransformToAxis(two, axis);
  float distance = glm::abs(glm::dot(toCenter, axis));
  return oneProjection + twoProjection - distance;
}

static inline bool TryAxis(const ZCollisionBox& one, const ZCollisionBox& two, glm::vec3 axis,
                           const glm::vec3& toCenter, unsigned int index,
                           float& smallestPenetration, unsigned int& smallestCase) {
  if (glm::length(axis) * glm::length(axis) < 0.0001f) return true;
  glm::vec3 normalizedAxis = glm::normalize(axis);

  float penetration = PenetrationOnAxis(one, two, axis, toCenter);
  if (penetration < 0) return false;
  if (penetration < smallestPenetration) {
    smallestPenetration = penetration;
    smallestCase = index;
  }
  return true;
}

static inline glm::vec3 ContactPoint(const glm::vec3& pOne, const glm::vec3& dOne, float oneSize,
                                     const glm::vec3& pTwo, const glm::vec3& dTwo, float twoSize, bool useOne) {
  glm::vec3 toSt, cOne, cTwo;
  float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
  float denom, mua, mub;

  smOne = glm::length(dOne) * glm::length(dOne);
  smTwo = glm::length(dTwo) * glm::length(dTwo);
  dpOneTwo = glm::dot(dTwo, dOne);

  toSt = pOne - pTwo;
  dpStaOne = glm::dot(dOne, toSt);
  dpStaTwo = glm::dot(dTwo, toSt);

  denom = smOne * smTwo - dpOneTwo * dpOneTwo;

  if (glm::abs(denom) < 0.0001f) { return useOne ? pOne : pTwo; }

  mua = (dpOne * dpStaTwo - smTwo * dpStaOne) / denom;
  mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

  if (mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize) {
    return useOne ? pOne : pTwo;
  } else {
    cOne = pOne + dOne * mua;
    cTwo = pTwo + dTwo * mub;
    return cOne * 0.5f + cTwo * 0.5f;
  }
}

void FillPointFaceBoxBox(const ZCollisionBox& one, const ZCollisionBox& two, const glm::vec3& toCenter,
                         ZCollisionData* data, unsigned int best, float pen) {
  Contact* contact = data->contacts;

  glm::Vec3 normal = one.Axis(best);
  if (glm::dot(one.Axis(best), toCenter) > 0) {
    normal = normal * -1.f;
  }

  glm::vec3 vertex = two.halfSize;
  if (glm::dot(two.Axis(0), normal) < 0) vertex.x = -vertex.x;
  if (glm::dot(two.Axis(1), normal) < 0) vertex.y = -vertex.y;
  if (glm::dot(two.Axis(2), normal) < 0) vertex.z = -vertex.z;

  contact->contactNormal = normal;
  contact->penetration = pen;
  contact->contactPoint = two.transform * vertex;
  contact->SetBodyData(one.body, two.body, data->friction, data->restitution);
}

bool ZIntersectionTests::SphereAndHalfSpace(const ZCollisionSphere& sphere, const ZCollisionPlane& plane) {
  float ballDistance = glm::dot(plane.direction, sphere.Axis(3)) - sphere.radius;
  return ballDistance <= plane.offset;
}

bool ZIntersectionTests::SphereAndSphere(const ZCollisionSphere&one, const ZCollisionSphere& two) {
  return glm::distance2(one.Axis(3), two, Axis(3)) < (one.radius + two.radius) * (one.radius + two.radius);
}

#define TEST_OVERLAP(axis) OverlapOnAxis(one, two, (axis), toCenter)
bool ZIntersectionTests::BoxAndBox(const ZCollisionBox& one, const ZCollisionBox& two) {
  glm::vec3 toCenter = two.Axis(3) - one.Axis(3);
  return (
    TEST_OVERLAP(one.Axis(0)) && TEST_OVERLAP(one.Axis(1)) && TEST_OVERLAP(one.Axis(2)) &&
    TEST_OVERLAP(two.Axis(0)) && TEST_OVERLAP(two.Axis(1)) && TEST_OVERLAP(two.Axis(2)) &&

    TEST_OVERLAP(glm::cross(one.Axis(0), two.Axis(0))) &&
    TEST_OVERLAP(glm::cross(one.Axis(0), two.Axis(1))) &&
    TEST_OVERLAP(glm::cross(one.Axis(0), two.Axis(2))) &&
    TEST_OVERLAP(glm::cross(one.Axis(1), two.Axis(0))) &&
    TEST_OVERLAP(glm::cross(one.Axis(1), two.Axis(1))) &&
    TEST_OVERLAP(glm::cross(one.Axis(1), two.Axis(2))) &&
    TEST_OVERLAP(glm::cross(one.Axis(2), two.Axis(0))) &&
    TEST_OVERLAP(glm::cross(one.Axis(2), two.Axis(1))) &&
    TEST_OVERLAP(glm::cross(one.Axis(2), two.Axis(2)))
  );
}
#undef TEST_OVERLAP

bool ZIntersectionTests::BoxAndHalfSpace(const ZCollisionBox& box, const ZCollisionPlane& plane) {
  float projectedRadius = TransformToAxis(box, plane.direction);
  float boxDistance = glm::dot(plane.direction, box.Axis(3)) - projectedRadius;
  return boxDistance <= plane.offset;
}

unsigned int ZFineCollisionDetector::SphereAndHalfSpace(const ZCollisionSphere& sphere, const ZCollisionPlane& plane, ZCollisionData* data) {
  if (data->contactsLeft <= 0) return 0;

  glm::vec3 position = sphere.Axis(3);

  float ballDistance = glm::dot(plane.direction, position) - sphere.radius - plane.offset;
  if (ballDistance >= 0) return 0;

  Contact* contact = data->contacts;
  contact->contactNormal = plane.direction;
  contact->penetration = -ballDistance;
  contact->contactPoint = position - plane.direction * (ballDistance + sphere.radius);
  contact->SetBodyData(sphere.body, nullptr, data->friction, data->restitution);

  data->AddContacts(1);
  return 1;
}

unsigned int ZFineCollisionDetector::SphereAndTruePlane(const ZCollisionSphere& sphere, const ZCollisionPlane& plane, ZCollisionData* data) {
  if (data->contactsLeft <= 0) return 0;

  glm::vec3 position = sphere.Axis(3);

  float centerDistance = glm::dot(plane.direction, position) - plane.offset;
  if (centerDistance * centerDistance > sphere.radius * sphere.radius) {
    return 0;
  }

  glm::vec3 normal = plane.direction;
  float penetration = -centerDistance;
  if (centerDistance < 0) {
    normal *= -1; penetration *= -1.f;
  }
  penetration += sphere.radius;

  Contact* contact = data->contacts;
  contact->contactNormal = normal;
  contact->penetration = penetration;
  contact->contactPoint = position - plane.direction * centerDistance;
  contact->SetBodyData(sphere.body, nullptr, data->friction, data->restitution);

  data->AddContacts(1);
  return 1;
}

unsigned int ZFineCollisionDetector::SphereAndSphere(const ZCollisionSphere& sphere, const ZCollisionSphere& sphere, ZCollisionData* data) {
  if (data->contactsLeft <= 0) return 0;

  glm::vec3 positionOne = one.Axis(3);
  glm::vec3 positionTwo = one.Axis(3);

  glm::vec3 midline = positionOne - positionTwo;
  float size = glm::distance2(midline);
  if (size <= 0.f || size >= one.radius + two.radius) return 0;

  glm::vec3 normal = midline * 1.f/size;

  Contact* contact = data->contacts;
  contact->contactNormal = normal;
  contact->contactPoint = positionOne + midline * 0.5f;
  contact->penetration = (one.radius + two.radius - size);
  contact->SetBodyData(one.body, two.body, data->friction, data->restitution);

  data->AddContacts(1);
  return 1;
}

unsigned int ZFineCollisionDetector::BoxAndHalfSpace(const ZCollisionBox& box, const ZCollisionSphere& sphere, ZCollisionData* data) {
  if (data->contactsLeft <= 0) return 0;

  if (!IntersectionTests::boxAndHalfSpace(box, plane)) return 0;

  static float mults[8][3] = {{1,1,1}, {-1,1,1}, {1,-1,1}, {-1,-1,1},
                              {1,1,-1}, {-1,1,-1}, {1,-1,-1}, {-1,-1,-1}};

  Contact* contact = data->contacts;
  unsigned contactsUsed = 0;
  for (unsigned i = 0; i < 8; i++) {
      glm::vec3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
      vertexPos *= box.halfSize;
      vertexPos = box.transform * vertexPos;

      float vertexDistance = glm::dot(vertexPos, plane.direction);

      if (vertexDistance <= plane.offset) {
          contact->contactPoint = plane.direction;
          contact->contactPoint *= (vertexDistance - plane.offset);
          contact->contactPoint += vertexPos;
          contact->contactNormal = plane.direction;
          contact->penetration = plane.offset - vertexDistance;

          contact->SetBodyData(box.body, nullptr, data->friction, data->restitution);

          contact++;
          contactsUsed++;
          if (contactsUsed == (unsigned int)data->contactsLeft) return contactsUsed;
      }
  }

  data->addContacts(contactsUsed);
  return contactsUsed;
}

#define CHECK_AXIS_OVERLAP(axis, index) \
  if (!TryAxis(one, two, (axis), toCenter, (index), pen, best)) return 0;
unsigned int ZFineCollisionDetector::BoxAndBox(const ZCollisionBox& one, const ZCollisionBox& two, ZCollisionData* data) {
  glm::vec3 toCenter = two.Axis(3) - one.Axis(3);

  float pen = std::numeric_limits<float>::max();
  unsigned int best = 0xffffff;

  CHECK_AXIS_OVERLAP(one.Axis(0), 0);
  CHECK_AXIS_OVERLAP(one.Axis(1), 1);
  CHECK_AXIS_OVERLAP(one.Axis(2), 2);

  CHECK_AXIS_OVERLAP(one.Axis(0), 3);
  CHECK_AXIS_OVERLAP(one.Axis(1), 4);
  CHECK_AXIS_OVERLAP(one.Axis(2), 5);

  unsigned int bestSingleAxis = best;

  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(0), two.Axis(0), 6);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(0), two.Axis(1), 7);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(0), two.Axis(2), 8);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(1), two.Axis(0), 9);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(1), two.Axis(1), 10);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(1), two.Axis(2), 11);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(2), two.Axis(0), 12);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(2), two.Axis(1), 13);
  CHECK_AXIS_OVERLAP(glm::cross(one.Axis(2), two.Axis(2), 14);

  assert(best != 0xffffff);

  if (best < 6) {
    FillPointFaceBoxBox(one, two, best < 3 ? toCenter : toCenter * -1.f, data, best < 3 ? best : best - 3, pen);
    data->AddContacts(1);
    return 1;
  } else {
    best -= 6;
    unsigned int oneAxisIndex = best / 3;
    unsigned int twoAxisIndex = best % 3;
    glm::vec3 oneAxis = one.Axis(oneAxisIndex);
    glm::vec3 twoAxis = two.Axis(twoAxisIndex);
    glm::vec3 axis = glm::normalize(glm::cross(oneAxis, twoAxis));

    if (glm::dot(axis, toCenter) > 0) axis = axis * -1.f;

    glm::vec3 ptOnOneEdge = one.halfSize;
    glm::vec3 ptOnTwoEdge = two.halfSize;
    for (unsigned i = 0; i < 3; i++) {
      if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
      else if (glm::dot(one.Axis(i), axis) > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

      if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
      else if (glm::dot(two.Axis(i), axis) > 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
    }

    ptOnOneEdge = one.transform * ptOnOneEdge;
    ptOnTwoEdge = one.transform * ptOnTwoEdge;

    glm::vec3 vertex = ContactPoint(ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
                                    ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
                                    bestSingleAxis > 2);

    Contact* contact = data->contacts;
    contact->penetration = pen;
    contact->contactNormal = axis;
    contact->contactPoint = vertex;
    contact->SetBodyData(one.body, two.body, data->friction, data->restitution);

    data->AddContacts(1);
    return 1;
  }
  return 0;
}
#undef CHECK_AXIS_OVERLAP

unsigned int ZFineCollisionDetector::BoxAndPoint(const ZCollisionBox& box, const glm::vec3& point, ZCollisionData* data) {
   glm::vec3 relativePoint = glm::inverse(box.transform) * point;
   glm::vec3 normal;

   float min_depth = box.halfSize.x - glm::abs(relativePoint.x);
   if (min_depth < 0) return 0;
   normal = box.Axis(0) * (relPt.x < 0 ? -1: 1);

   float depth = box.halfSize.y - glm::abs(relativePoint.y);
   if (depth < 0) return 0;
   else if (depth < min_depth) {
       min_depth = depth;
       normal = box.Axis(1) * (relativePoint.y < 0 ? -1 : 1);
   }

   depth = box.halfSize.z - glm::abs(relativePoint.z);
   if (depth < 0) return 0;
   else if (depth < min_depth) {
       min_depth = depth;
       normal = box.Axis(2) * (relativePoint.z < 0 ? -1 : 1);
   }

   Contact* contact = data->contacts;
   contact->contactNormal = normal;
   contact->contactPoint = point;
   contact->penetration = min_depth;

   contact->setBodyData(box.body, nullptr, data->friction, data->restitution);

   data->addContacts(1);
   return 1;
}

unsigned int ZFineCollisionDetector::BoxAndSphere(const ZCollisionBox& box, const ZCollisionSphere& sphere, ZCollisionData* data) {
  glm::vec3 centre = sphere.Axis(3);
  glm::vec3 relCentre = glm::inverse(box.transform) * centre;

  if (glm::abs(relCentre.x) - sphere.radius > box.halfSize.x ||
      glm::abs(relCentre.y) - sphere.radius > box.halfSize.y ||
      glm::abs(relCentre.z) - sphere.radius > box.halfSize.z) {
      return 0;
  }

  float dist;
  glm::vec3 closestPt(0.f);

  dist = relCentre.x;
  if (dist > box.halfSize.x) dist = box.halfSize.x;
  if (dist < -box.halfSize.x) dist = -box.halfSize.x;
  closestPt.x = dist;

  dist = relCentre.y;
  if (dist > box.halfSize.y) dist = box.halfSize.y;
  if (dist < -box.halfSize.y) dist = -box.halfSize.y;
  closestPt.y = dist;

  dist = relCentre.z;
  if (dist > box.halfSize.z) dist = box.halfSize.z;
  if (dist < -box.halfSize.z) dist = -box.halfSize.z;
  closestPt.z = dist;

  dist = glm::distance2(closestPt, relCentre);
  if (dist > sphere.radius * sphere.radius) return 0;

  glm::vec3 closestPtWorld = box.transform * closestPt;

  Contact* contact = data->contacts;
  contact->contactNormal = glm::normalize(closestPtWorld - centre);
  contact->contactPoint = closestPtWorld;
  contact->penetration = sphere.radius - glm::sqrt(dist);
  contact->SetBodyData(box.body, sphere.body, data->friction, data->restitution);

  data->addContacts(1);
  return 1;
}
