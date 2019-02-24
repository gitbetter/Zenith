//
//  ZContact.hpp
//  Zenith
//
//  Created by <author> on 22/02/2019.
//
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
class ZContactResolver;
class ZGameObject;
class ZContact;

// Class and Data Structure Definitions
typedef std::vector<std::shared_ptr<ZContact>> ContactPtrList;

class ZContact {
  friend class ZContactResolver;
public:

  ZGameObject* body[2];
  float friction;
  float restitution;
  glm::vec3 contactPoint;
  glm::vec3 contactNormal;
  float penetration;

  void SetBodyData(ZGameObject* one, ZGameObject* two, float friction, float restitution);

protected:

  glm::mat3 contactToWorld;
  glm::vec3 contactVelocity;
  float desiredDeltaVelocity;
  glm::vec3 relativeContactPosition[2];

  void MatchAwakeState();
  void SwapBodies();
  void CalculateContactBasis();
  glm::vec3 CalculateLocalVelocity(unsigned int bodyIndex, float duration);
  void CalculateDesiredDeltaVelocity(float duration);
  void CalculateInternals(float duration);
  void ApplyImpulse(const glm::vec3& impulse, ZGameObject* body, glm::vec3* velocityChange, glm::vec3* rotationChange);
  void ApplyVelocityChange(glm::vec3 velocityChange[2], glm::vec3 rotationChange[2]);
  void ApplyPositionChange(glm::vec3 linearChange[2], glm::vec3 angularChange[2], float penetration);
  glm::vec3 CalculateFrictionlessImpulse(glm::mat3* inverseInertiaTensor);
  glm::vec3 CalculateFrictionImpulse(glm::mat3* inverseInertiaTensor);

};

class ZContactResolver {
private:

    bool validSettings;

protected:

    unsigned int velocityIterations;
    unsigned int positionIterations;
    float velocityEpsilon;
    float positionEpsilon;

    void PrepareContacts(ContactPtrList contactArray, float duration);

    void AdjustVelocities(ContactPtrList contactArray, float duration);

    void AdjustPositions(ContactPtrList contacts, float duration);

public:

    unsigned int velocityIterationsUsed;
    unsigned int positionIterationsUsed;

    ZContactResolver(unsigned int iterations,
                     float velocityEpsilon = 0.01f,
                     float positionEpsilon = 0.01f);

    ZContactResolver(unsigned int velocityIterations,
                     unsigned int positionIterations,
                     float velocityEpsilon = 0.01f,
                     float positionEpsilon = 0.01f);

    bool IsValid() {
        return (velocityIterations > 0) &&
               (positionIterations > 0) &&
               (positionEpsilon >= 0.0f) &&
               (positionEpsilon >= 0.0f);
    }

    void SetIterations(unsigned int velocityIterations, unsigned int positionIterations);

    void SetIterations(unsigned int iterations);

    void SetEpsilon(float velocityEpsilon, float positionEpsilon);

    void ResolveContacts(ContactPtrList contactArray, float duration);
};

class ZContactGenerator {
public:

  virtual unsigned AddContact(ZContact* contact, unsigned int limit) const = 0;
};
