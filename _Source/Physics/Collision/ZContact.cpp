//
//  ZContact.cpp
//  Zenith
//
//  Created by <author> on 22/02/2019.
//
//

#include "ZContact.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsComponent.hpp"
#include <glm/gtx/matrix_cross_product.hpp>

void ZContact::SetBodyData(ZGameObject* one, ZGameObject* two, float friction, float restitution) {
  this->body[0] = one;
  this->body[1] = two;
  this->friction = friction;
  this->restitution = restitution;
}

void ZContact::MatchAwakeState() {
  if (!body[1]) return;

  ZPhysicsComponent* physicsComp0 = body[0]->FindComponent<ZPhysicsComponent>();
  ZPhysicsComponent* physicsComp1 = body[1]->FindComponent<ZPhysicsComponent>();

  if (!physicsComp0 || !physicsComp1) return;

  bool body0awake = physicsComp0->Awake();
  bool body1awake = physicsComp1->Awake();

  if (body0awake ^ body1awake) {
      if (body0awake) physicsComp1->Awake();
      else physicsComp0->Awake();
  }
}

void ZContact::SwapBodies() {
  contactNormal *= -1;

  ZGameObject* temp = body[0];
  body[0] = body[1];
  body[1] = temp;
}

inline void ZContact::CalculateContactBasis() {
    glm::vec3 contactTangent[2];

    if (glm::abs(contactNormal.x) > glm::abs(contactNormal.y)) {
      const float s = 1.0f/glm::sqrt(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);

      // The new X-axis is at right angles to the world Y-axis
      contactTangent[0].x = contactNormal.z * s;
      contactTangent[0].y = 0;
      contactTangent[0].z = -contactNormal.x * s;

      // The new Y-axis is at right angles to the new X- and Z- axes
      contactTangent[1].x = contactNormal.y * contactTangent[0].x;
      contactTangent[1].y = contactNormal.z * contactTangent[0].x - contactNormal.x * contactTangent[0].z;
      contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
    } else {
      const float s = 1.0f/glm::sqrt(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);

      contactTangent[0].x = 0;
      contactTangent[0].y = -contactNormal.z * s;
      contactTangent[0].z = contactNormal.y * s;

      contactTangent[1].x = contactNormal.y * contactTangent[0].z - contactNormal.z * contactTangent[0].y;
      contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
      contactTangent[1].z = contactNormal.x * contactTangent[0].y;
    }

    contactToWorld = glm::mat3(contactNormal, contactTangent[0], contactTangent[1]);
}

void ZContact::CalculateInternals(float duration) {
    if (!body[0]) SwapBodies();
    assert(body[0]);

    CalculateContactBasis();

    relativeContactPosition[0] = contactPoint - body[0]->Position();
    if (body[1]) {
        relativeContactPosition[1] = contactPoint - body[1]->Position();
    }

    contactVelocity = CalculateLocalVelocity(0, duration);
    if (body[1]) {
        contactVelocity -= CalculateLocalVelocity(1, duration);
    }

    CalculateDesiredDeltaVelocity(duration);
}

glm::vec3 ZContact::CalculateLocalVelocity(unsigned int bodyIndex, float duration) {
    ZGameObject* thisBody = body[bodyIndex];

    ZPhysicsComponent* physicsComponent = thisBody->FindComponent<ZPhysicsComponent>();
    if (physicsComponent == nullptr) return glm::vec3(0.f);

    glm::vec3 velocity = glm::cross(physicsComponent->AngularVelocity(), relativeContactPosition[bodyIndex]);
    velocity += physicsComponent->Velocity();

    glm::vec3 accVelocity = physicsComponent->PreviousAcceleration() * duration;
    accVelocity = glm::transpose(contactToWorld) * accVelocity;
    accVelocity.x = 0;

    glm::vec3 contactVelocity = glm::transpose(contactToWorld) * velocity;
    contactVelocity += accVelocity;

    return contactVelocity;
}

void ZContact::CalculateDesiredDeltaVelocity(float duration) {
    const static float velocityLimit = 0.25f;

    ZPhysicsComponent* physicsComp0 = body[0]->FindComponent<ZPhysicsComponent>();
    if (physicsComp0 == nullptr) return;

    float velocityFromAcc = 0.f;

    if (physicsComp0->Awake()) {
      velocityFromAcc += glm::dot(physicsComp0->PreviousAcceleration(), contactNormal * duration);
    }

    if (body[1] && body[1]->FindComponent<ZPhysicsComponent>()) {
        ZPhysicsComponent* physicsComp1 = body[1]->FindComponent<ZPhysicsComponent>();
        if (physicsComp1->Awake()) {
          velocityFromAcc -= glm::dot(physicsComp1->PreviousAcceleration(), contactNormal) * duration;
        }
    }

    float thisRestitution = restitution;
    if (glm::abs(contactVelocity.x) < velocityLimit) {
        thisRestitution = 0.0f;
    }

    desiredDeltaVelocity = -contactVelocity.x - thisRestitution * (contactVelocity.x - velocityFromAcc);
}

void ZContact::ApplyVelocityChange(glm::vec3 velocityChange[2], glm::vec3 rotationChange[2]) {
  ZPhysicsComponent* physicsComp0 = body[0]->FindComponent<ZPhysicsComponent>();
  ZPhysicsComponent* physicsComp1 = body[1] ? body[1]->FindComponent<ZPhysicsComponent>() : nullptr;

  if (physicsComp0 == nullptr) return;

  glm::mat3 inverseInertiaTensor[2];
  inverseInertiaTensor[0] = glm::mat3(body[0]->ModelMatrix()) * physicsComp0->InverseInertiaTensor();
  if (body[1] && physicsComp1 != nullptr) {
    inverseInertiaTensor[1] = glm::mat3(body[1]->ModelMatrix()) * physicsComp1->InverseInertiaTensor();
  }

  glm::vec3 impulseContact;
  if (friction == 0.0f) {
      impulseContact = CalculateFrictionlessImpulse(inverseInertiaTensor);
  } else {
      impulseContact = CalculateFrictionImpulse(inverseInertiaTensor);
  }

  glm::vec3 impulse = contactToWorld * impulseContact;

  glm::vec3 impulsiveTorque = glm::cross(relativeContactPosition[0], impulse);
  rotationChange[0] = inverseInertiaTensor[0] * impulsiveTorque;
  velocityChange[0] = glm::vec3(0.f);
  velocityChange[0] += impulse * physicsComp0->InverseMass();

  physicsComp0->SetVelocity(physicsComp0->Velocity() + velocityChange[0]);
  physicsComp0->SetAngularVelocity(physicsComp0->AngularVelocity() + rotationChange[0]);

  if (body[1] && physicsComp1 != nullptr) {
      glm::vec3 impulsiveTorque = glm::cross(impulse, relativeContactPosition[1]);
      rotationChange[1] = inverseInertiaTensor[1] * impulsiveTorque;
      velocityChange[1] = glm::vec3(0.f);
      velocityChange[1] += impulse * -physicsComp1->InverseMass();

      physicsComp1->SetVelocity(physicsComp1->Velocity() + velocityChange[1]);
      physicsComp1->SetAngularVelocity(physicsComp1->AngularVelocity() + rotationChange[1]);
  }
}

inline glm::vec3 ZContact::CalculateFrictionlessImpulse(glm::mat3* inverseInertiaTensor)
{
    ZPhysicsComponent* physicsComp0 = body[0]->FindComponent<ZPhysicsComponent>();
    if (physicsComp0 == nullptr) return glm::vec3(0.f);

    glm::vec3 impulseContact;

    glm::vec3 deltaVelWorld = glm::cross(relativeContactPosition[0], contactNormal);
    deltaVelWorld = inverseInertiaTensor[0] * deltaVelWorld;
    deltaVelWorld = glm::cross(deltaVelWorld, relativeContactPosition[0]);

    float deltaVelocity = glm::dot(deltaVelWorld, contactNormal);
    deltaVelocity += physicsComp0->InverseMass();

    if (body[1] && body[1]->FindComponent<ZPhysicsComponent>()) {
        ZPhysicsComponent* physicsComp1 = body[1]->FindComponent<ZPhysicsComponent>();

        glm::vec3 deltaVelWorld = glm::cross(relativeContactPosition[1], contactNormal);
        deltaVelWorld = inverseInertiaTensor[1] * deltaVelWorld;
        deltaVelWorld = glm::cross(deltaVelWorld, relativeContactPosition[1]);

        deltaVelocity += glm::dot(deltaVelWorld, contactNormal);

        deltaVelocity += physicsComp1->InverseMass();
    }

    impulseContact.x = desiredDeltaVelocity / deltaVelocity;
    impulseContact.y = 0;
    impulseContact.z = 0;
    return impulseContact;
}

inline glm::vec3 ZContact::CalculateFrictionImpulse(glm::mat3* inverseInertiaTensor)
{
  ZPhysicsComponent* physicsComp0 = body[0]->FindComponent<ZPhysicsComponent>();
  if (physicsComp0 == nullptr) return glm::vec3(0.f);

  glm::vec3 impulseContact;
  float inverseMass = physicsComp0->InverseMass();

  glm::mat3 impulseToTorque = glm::matrixCross3(relativeContactPosition[0]);

  glm::mat3 deltaVelWorld = impulseToTorque;
  deltaVelWorld *= inverseInertiaTensor[0];
  deltaVelWorld *= impulseToTorque;
  deltaVelWorld *= -1.f;

  if (body[1] && body[1]->FindComponent<ZPhysicsComponent>()) {
    ZPhysicsComponent* physicsComp1 = body[1]->FindComponent<ZPhysicsComponent>();

    impulseToTorque = glm::matrixCross3(relativeContactPosition[1]);

    glm::mat3 deltaVelWorld2 = impulseToTorque;
    deltaVelWorld2 *= inverseInertiaTensor[1];
    deltaVelWorld2 *= impulseToTorque;
    deltaVelWorld2 *= -1.f;

    deltaVelWorld += deltaVelWorld2;

    inverseMass += physicsComp1->InverseMass();
  }

  glm::mat3 deltaVelocity = glm::transpose(contactToWorld);
  deltaVelocity *= deltaVelWorld;
  deltaVelocity *= contactToWorld;

  deltaVelocity[0][0] += inverseMass;
  deltaVelocity[1][1] += inverseMass;
  deltaVelocity[2][2] += inverseMass;

  glm::mat3 impulseMatrix = glm::inverse(deltaVelocity);

  glm::vec3 velKill(desiredDeltaVelocity, -contactVelocity.y, -contactVelocity.z);

  impulseContact = impulseMatrix * velKill;

  float planarImpulse = glm::sqrt(impulseContact.y * impulseContact.y + impulseContact.z * impulseContact.z);
  if (planarImpulse > impulseContact.x * friction) {
      // Static friction exceeded, so we need to use kinetic friction
      impulseContact.y /= planarImpulse;
      impulseContact.z /= planarImpulse;

      impulseContact.x = deltaVelocity[0][0] +
          deltaVelocity[0][1] * friction * impulseContact.y +
          deltaVelocity[0][2] * friction * impulseContact.z;
      impulseContact.x = desiredDeltaVelocity / impulseContact.x;
      impulseContact.y *= friction * impulseContact.x;
      impulseContact.z *= friction * impulseContact.x;
  }
  return impulseContact;
}

void ZContact::ApplyPositionChange(glm::vec3 linearChange[2],
                                   glm::vec3 angularChange[2],
                                   float penetration) {
    const float angularLimit = 0.2f;
    float angularMove[2];
    float linearMove[2];

    float totalInertia = 0;
    float linearInertia[2];
    float angularInertia[2];

    for (unsigned i = 0; i < 2; i++) if (body[i]) {
      ZPhysicsComponent* physicsComp = body[i]->FindComponent<ZPhysicsComponent>();
      if (physicsComp == nullptr) continue;

      glm::mat3 inverseInertiaTensor = glm::mat3(body[i]->ModelMatrix()) * physicsComp->InverseInertiaTensor();

      glm::vec3 angularInertiaWorld = glm::cross(relativeContactPosition[i], contactNormal);
      angularInertiaWorld = inverseInertiaTensor * angularInertiaWorld;
      angularInertiaWorld = glm::cross(angularInertiaWorld, relativeContactPosition[i]);
      angularInertia[i] = glm::dot(angularInertiaWorld, contactNormal);

      linearInertia[i] = physicsComp->InverseMass();

      totalInertia += linearInertia[i] + angularInertia[i];
    }

    for (unsigned i = 0; i < 2; i++) if (body[i]) {
      ZPhysicsComponent* physicsComp = body[i]->FindComponent<ZPhysicsComponent>();
      if (physicsComp == nullptr) continue;

      float sign = i == 0 ? 1.f : -1.f;
      angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
      linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);

      glm::vec3 projection = relativeContactPosition[i];
      projection += contactNormal * glm::dot(-relativeContactPosition[i], contactNormal);

      float maxMagnitude = angularLimit * glm::length(projection);

      if (angularMove[i] < -maxMagnitude) {
        float totalMove = angularMove[i] + linearMove[i];
        angularMove[i] = -maxMagnitude;
        linearMove[i] = totalMove - angularMove[i];
      } else if (angularMove[i] > maxMagnitude) {
        float totalMove = angularMove[i] + linearMove[i];
        angularMove[i] = maxMagnitude;
        linearMove[i] = totalMove - angularMove[i];
      }

      if (angularMove[i] == 0) {
          angularChange[i] = glm::vec3(0.f);
      } else {
          glm::vec3 targetAngularDirection = glm::cross(relativeContactPosition[i], contactNormal);
          glm::mat3 inverseInertiaTensor = glm::mat3(body[i]->ModelMatrix()) * physicsComp->InverseInertiaTensor();

          angularChange[i] = (inverseInertiaTensor * targetAngularDirection) * (angularMove[i] / angularInertia[i]);
      }

      linearChange[i] = contactNormal * linearMove[i];

      body[i]->SetPosition(body[i]->Position() + contactNormal * linearMove[i]);
      body[i]->SetOrientation(body[i]->Orientation() * angularChange[i]);

      if (!physicsComp->Awake()) body[i]->CalculateDerivedData();
    }
}

ZContactResolver::ZContactResolver(unsigned int iterations, float velocityEpsilon, float positionEpsilon) {
  SetIterations(iterations);
  SetEpsilon(velocityEpsilon, positionEpsilon);
}

ZContactResolver::ZContactResolver(unsigned int velocityIterations, unsigned int positionIterations,
                                  float velocityEpsilon, float positionEpsilon) {
    SetIterations(velocityIterations, positionIterations);
    SetEpsilon(velocityEpsilon, positionEpsilon);
}

void ZContactResolver::SetIterations(unsigned int iterations) {
    SetIterations(iterations, iterations);
}

void ZContactResolver::SetIterations(unsigned int velocityIterations, unsigned int positionIterations) {
    ZContactResolver::velocityIterations = velocityIterations;
    ZContactResolver::positionIterations = positionIterations;
}


void ZContactResolver::SetEpsilon(float velocityEpsilon, float positionEpsilon) {
    ZContactResolver::velocityEpsilon = velocityEpsilon;
    ZContactResolver::positionEpsilon = positionEpsilon;
}

void ZContactResolver::ResolveContacts(ContactPtrList contacts, float duration) {
    if (contacts.size() == 0) return;
    if (!IsValid()) return;

    PrepareContacts(contacts, duration);
    AdjustPositions(contacts, duration);
    AdjustVelocities(contacts, duration);
}

void ZContactResolver::PrepareContacts(ContactPtrList contacts, float duration) {
    std::shared_ptr<ZContact> lastContact = contacts.back();
    for (ContactPtrList::iterator it = contacts.begin(); it != contacts.end(); it++) {
        (*it)->CalculateInternals(duration);
    }
}

void ZContactResolver::AdjustPositions(ContactPtrList contacts, float duration) {
  ContactPtrList::iterator it = contacts.end();
  glm::vec3 linearChange[2], angularChange[2];
  glm::vec3 deltaPosition;
  float max;

  positionIterationsUsed = 0;
  while (positionIterationsUsed < positionIterations) {
    max = positionEpsilon;
    for (ContactPtrList::iterator i = contacts.begin(); i != contacts.end(); i++) {
      if ((*i)->penetration > max) {
        max = (*i)->penetration;
        it = i;
      }
    }

    if (it == contacts.end()) break;

    (*it)->MatchAwakeState();

    (*it)->ApplyPositionChange(linearChange, angularChange, max);

    for (ContactPtrList::iterator i = contacts.begin(); i != contacts.end(); i++) {
      for (unsigned int b = 0; b < 2; b++) if ((*i)->body[b]) {
        for (unsigned int d = 0; d < 2; d++) {
          if ((*i)->body[b] == (*it)->body[d]) {
              deltaPosition = linearChange[d] + glm::cross(angularChange[d], (*i)->relativeContactPosition[b]);
              (*i)->penetration += glm::dot(deltaPosition, (*i)->contactNormal) * (b ? 1.f : -1.f);
          }
        }
      }
    }
    positionIterationsUsed++;
  }
}

void ZContactResolver::AdjustVelocities(ContactPtrList contacts, float duration) {
  glm::vec3 velocityChange[2], rotationChange[2];
  glm::vec3 deltaVel;

  velocityIterationsUsed = 0;
  while (velocityIterationsUsed < velocityIterations) {
    float max = velocityEpsilon;
    ContactPtrList::iterator it = contacts.end();
    for (ContactPtrList::iterator i = contacts.begin(); i != contacts.end(); i++) {
      if ((*i)->desiredDeltaVelocity > max) {
        max = (*i)->desiredDeltaVelocity;
        it = i;
      }
    }

    if (it == contacts.end()) break;

    (*it)->MatchAwakeState();

    (*it)->ApplyVelocityChange(velocityChange, rotationChange);

    // Update any incident bodies
    for (ContactPtrList::iterator i = contacts.begin(); i != contacts.end(); i++) {
      for (unsigned int b = 0; b < 2; b++) {
        if ((*i)->body[b]) {
          for (unsigned int d = 0; d < 2; d++) {
            if ((*i)->body[b] == (*it)->body[d]) {
              deltaVel = velocityChange[d] + glm::cross(rotationChange[d], (*i)->relativeContactPosition[b]);

              (*i)->contactVelocity += glm::transpose((*i)->contactToWorld) * deltaVel * (b ? -1.f : 1.f);
              (*i)->CalculateDesiredDeltaVelocity(duration);
            }
          }
        }
      }
    }
    velocityIterationsUsed++;
  }
}
