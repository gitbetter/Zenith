//
//  ZObjectForceRegistry.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZObjectForceRegistry.hpp"
#include "ZGameObject.hpp"
#include "ZForceGenerator.hpp"

void ZObjectForceRegistry::Add(ZGameObject* object, ZForceGenerator* generator) {
  ZObjectForceRegistration registration;
  registration.object = object;
  registration.forceGenerator = generator;
  registrations_.push_back(registration);
}

void ZObjectForceRegistry::Remove(ZGameObject* object, ZForceGenerator* generator) {
  registrations_.erase(std::remove_if(registrations_.begin(), registrations_.end(), [&](ZObjectForceRegistration registration) {
      return registration.object == object && registration.forceGenerator == generator;
  }), registrations_.end());
}

void ZObjectForceRegistry::Clear() {
  registrations_.clear();
}

void ZObjectForceRegistry::UpdateForces() {
  for (ZOFRegistry::iterator reg = registrations_.begin(); reg != registrations_.end(); reg++) {
    reg->forceGenerator->UpdateForce(reg->object);
  }
}
