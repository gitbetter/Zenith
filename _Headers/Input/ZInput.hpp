//
//  ZInput.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
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

  virtual void Register(std::shared_ptr<ZObject> gameObject);
  virtual void Broadcast(const ZInputCommand& command);
  virtual void Process() = 0;
  virtual void GetPointerPosition(double& x, double& y) = 0;
protected:

  std::vector<std::shared_ptr<ZObject>> registeredObjects_;
};
