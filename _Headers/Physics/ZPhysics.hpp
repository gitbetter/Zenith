//
//  ZPhysics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>

// Forward Declarations
class ZObjectForceRegistry;
class ZGameObject;
class ZCamera;

// Class and Data Structure Definitions
class ZPhysics {
private:
public:
  ZPhysics() { }
  ~ZPhysics() { }

  void Initialize();

  void Update(std::vector<ZGameObject*> gameObjects, std::vector<ZCamera*> gameCameras);

  void CleanUp() { }

  ZObjectForceRegistry* Registry() { return registry_; }

protected:
  ZObjectForceRegistry* registry_ = nullptr;
};
