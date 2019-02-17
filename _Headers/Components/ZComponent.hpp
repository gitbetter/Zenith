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

// Class and Data Structure Definitions
class ZComponent : public ZObject {
friend class ZGameObject;
private:
public:
  ZComponent() {
    id_ = "ZC_" + ZEngine::IDSequence()->Next();
  }
  virtual ~ZComponent() { }

  ZGameObject* Object() { return object_; }

protected:
  ZGameObject* object_;
};
