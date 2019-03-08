//
//  ZObjectForceRegistry.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZGameObject.hpp"

// Forward Declarations
class ZGameObject;
class ZForceGenerator;

// Class and Data Structure Definitions
class ZObjectForceRegistry {
private:

public:

    ~ZObjectForceRegistry() { }

    void Add(ZGameObject* object, ZForceGenerator* generator);
    void Remove(ZGameObject* object, ZForceGenerator* generator);
    void Clear();
    void UpdateForces();

protected:

  struct ZObjectForceRegistration {
    ZGameObject* object;
    ZForceGenerator* forceGenerator;
  };
  typedef std::vector<ZObjectForceRegistration> ZOFRegistry;
  ZOFRegistry registrations_;
};
