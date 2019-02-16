//
//  ZComponent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZEngine.hpp"
#include "ZObject.hpp"

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
class ZComponent : public ZObject {
friend class ZGameObject;
private:
public:
  ZComponent() {
    id_ = "ZC_" + ZEngine::NewId();
  }
  virtual ~ZComponent() { }

  ZGameObject* Object() { return object_; }

protected:
  ZGameObject* object_;
};
