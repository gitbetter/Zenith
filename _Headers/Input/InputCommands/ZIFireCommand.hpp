//
//  ZIEscapeCommand.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZInputCommand.hpp"

// Forward Declarations
class ZObject;

// Class and Data Structure Definitions
class ZIFireCommand : public ZInputCommand {
private:

public:

  ZIFireCommand() { }
  virtual void Execute(std::shared_ptr<ZObject> gameObject) const override;
protected:


};
