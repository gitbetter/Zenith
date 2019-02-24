//
//  ZPhysics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZFineCollisionDetector.hpp"

// Forward Declarations
class ZObjectForceRegistry;

// Class and Data Structure Definitions
class ZPhysics {
private:

public:

  ZPhysics() { }
  ~ZPhysics() { }

  void Initialize();

  void UpdateForces();
  void GenerateContacts(ZGameObjectMap gameObjects);
  void ResolveContacts();

  void CleanUp() { }

  ZObjectForceRegistry* Registry() { return registry_; }

protected:
  const static unsigned maxContacts_ = 256;

  ZObjectForceRegistry* registry_ = nullptr;
  ZContactResolver* contactResolver_ = nullptr;
  ZCollisionData collisionData_;

};
