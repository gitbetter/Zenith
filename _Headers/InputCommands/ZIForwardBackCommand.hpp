//
//  ZIForwardBackCommand.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZInputCommand.hpp"

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
class ZIForwardBackCommand : public ZInputCommand {
private:
public:
  ZIForwardBackCommand(float controlThrow) : ZInputCommand(controlThrow) { }
  virtual void Execute(ZGameObject* gameObject) const override;
protected:

};
