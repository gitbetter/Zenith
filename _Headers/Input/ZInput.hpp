//
//  ZInput.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>

// Forward Declarations
class ZObject;
class ZInputCommand;

// Class and Data Structure Definitions
class ZInput {
private:

public:

  virtual ~ZInput() { }

  virtual void Register(ZObject* gameObject);
  virtual void Broadcast(const ZInputCommand& command);
  virtual void Process() = 0;
protected:

  std::vector<ZObject*> registeredObjects_;
};
