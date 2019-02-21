//
//  ZComponent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZIDSequence.hpp"
#include "ZEngine.hpp"
#include "ZObject.hpp"

// Forward Declarations
class ZGameObject;
struct ZOFNode;

// Class and Data Structure Definitions
class ZComponent : public ZObject {
friend class ZGameObject;
private:
public:
  virtual ~ZComponent() { }

  virtual void Initialize(ZOFNode* root) = 0;

  ZGameObject* Object() { return object_; }

protected:
  ZGameObject* object_;
};
