//
//  ZForceGenerator.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 14/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGameObject.hpp"

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
class ZForceGenerator {
private:

public:

  virtual ~ZForceGenerator() { }

  virtual void UpdateForce(ZGameObject* object) = 0;

protected:


};
