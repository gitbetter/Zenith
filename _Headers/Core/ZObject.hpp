//
//  ZObject.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZObject {
private:

public:

  virtual ~ZObject() { }

  virtual void Update() { };
  virtual void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) { };

  std::string ID() { return id_; }

protected:

  std::string id_;
  
};
