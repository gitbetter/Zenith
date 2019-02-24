//
//  ZPhysics.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZPhysics.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZObjectForceRegistry.hpp"
#include "ZGameObject.hpp"
#include "ZCollisionComponent.hpp"

void ZPhysics::Initialize() {
  if (registry_ == nullptr) {
    registry_ = new ZObjectForceRegistry;
  }

  if (contactResolver_ == nullptr) {
    contactResolver_ = new ZContactResolver(maxContacts_ * 8);
  }

  collisionData_.contacts = ContactPtrList();
}

void ZPhysics::UpdateForces() {
  registry_->UpdateForces();
}

void ZPhysics::GenerateContacts(ZGameObjectMap gameObjects) {
  collisionData_.Reset(maxContacts_);
  collisionData_.friction = 0.8f;
  collisionData_.restitution = 0.3f;
  collisionData_.tolerance = 0.1f;

  // Perform exhaustive collision detection
  // TODO: Look into other acceleration structures to improve performance
  for (ZGameObjectMap::iterator it = gameObjects.begin(); it != gameObjects.end(); it++) {
    ZCollisionComponent* one = it->second->FindComponent<ZCollisionComponent>();
    if (!one) continue;

    for (ZGameObjectMap::iterator otherIt = std::next(it, 1); otherIt != gameObjects.end(); otherIt++) {
      ZCollisionComponent* two = otherIt->second->FindComponent<ZCollisionComponent>();
      if (!two) continue;

      ZCollisionPrimitive* primitive1 = one->CollisionPrimitive();
      ZCollisionPrimitive* primitive2 = two->CollisionPrimitive();
      if (dynamic_cast<ZCollisionBox*>(primitive1) && dynamic_cast<ZCollisionPlane*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionBox* box = dynamic_cast<ZCollisionBox*>(primitive1);
        ZCollisionPlane* plane = dynamic_cast<ZCollisionPlane*>(primitive2);
        ZFineCollisionDetector::BoxAndHalfSpace(*box, *plane, &collisionData_);
      } else if (dynamic_cast<ZCollisionPlane*>(primitive1) && dynamic_cast<ZCollisionBox*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionPlane* plane = dynamic_cast<ZCollisionPlane*>(primitive1);
        ZCollisionBox* box = dynamic_cast<ZCollisionBox*>(primitive2);
        ZFineCollisionDetector::BoxAndHalfSpace(*box, *plane, &collisionData_);
      } else if (dynamic_cast<ZCollisionBox*>(primitive1) && dynamic_cast<ZCollisionBox*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionBox* boxOne = dynamic_cast<ZCollisionBox*>(primitive1);
        ZCollisionBox* boxTwo = dynamic_cast<ZCollisionBox*>(primitive2);
        ZFineCollisionDetector::BoxAndBox(*boxOne, *boxTwo, &collisionData_);
      } else if (dynamic_cast<ZCollisionBox*>(primitive1) && dynamic_cast<ZCollisionSphere*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionBox* box = dynamic_cast<ZCollisionBox*>(primitive1);
        ZCollisionSphere* sphere = dynamic_cast<ZCollisionSphere*>(primitive2);
        ZFineCollisionDetector::BoxAndSphere(*box, *sphere, &collisionData_);
      } else if (dynamic_cast<ZCollisionSphere*>(primitive1) && dynamic_cast<ZCollisionBox*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionSphere* sphere = dynamic_cast<ZCollisionSphere*>(primitive1);
        ZCollisionBox* box = dynamic_cast<ZCollisionBox*>(primitive2);
        ZFineCollisionDetector::BoxAndSphere(*box, *sphere, &collisionData_);
      } else if (dynamic_cast<ZCollisionSphere*>(primitive1) && dynamic_cast<ZCollisionPlane*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionSphere* sphere = dynamic_cast<ZCollisionSphere*>(primitive1);
        ZCollisionPlane* plane = dynamic_cast<ZCollisionPlane*>(primitive2);
        ZFineCollisionDetector::SphereAndHalfSpace(*sphere, *plane, &collisionData_);
      } else if (dynamic_cast<ZCollisionPlane*>(primitive1) && dynamic_cast<ZCollisionSphere*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionPlane* plane = dynamic_cast<ZCollisionPlane*>(primitive1);
        ZCollisionSphere* sphere = dynamic_cast<ZCollisionSphere*>(primitive2);
        ZFineCollisionDetector::SphereAndHalfSpace(*sphere, *plane, &collisionData_);
      } else if (dynamic_cast<ZCollisionSphere*>(primitive1) && dynamic_cast<ZCollisionSphere*>(primitive2)) {
        if (!collisionData_.HasMoreContacts()) return;
        ZCollisionSphere* sphereOne = dynamic_cast<ZCollisionSphere*>(primitive1);
        ZCollisionSphere* sphereTwo = dynamic_cast<ZCollisionSphere*>(primitive2);
        ZFineCollisionDetector::SphereAndSphere(*sphereOne, *sphereTwo, &collisionData_);
      }
    }
  }
}

void ZPhysics::ResolveContacts() {
  contactResolver_->ResolveContacts(collisionData_.contacts, ZEngine::UPDATE_STEP_SIZE);
}
