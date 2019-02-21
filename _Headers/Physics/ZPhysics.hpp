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

// Forward Declarations
class ZObjectForceRegistry;
class ZGameObject;

// Class and Data Structure Definitions
class ZPhysics {
private:
public:
  ZPhysics() { }
  ~ZPhysics() { }

  void Initialize();

  void Update();

  void CleanUp() { }

  ZObjectForceRegistry* Registry() { return registry_; }

protected:
  ZObjectForceRegistry* registry_ = nullptr;
};
