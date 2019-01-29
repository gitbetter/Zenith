//
//  ZInputCommand.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <iostream>

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
class ZInputCommand {
private:
public:
  ZInputCommand(float controlThrow = 0.0) : controlThrow_(controlThrow) { }
  virtual ~ZInputCommand() { }
  virtual void Execute(ZGameObject* gameObject) const = 0;
protected:
  float controlThrow_ = 0.0;
};
