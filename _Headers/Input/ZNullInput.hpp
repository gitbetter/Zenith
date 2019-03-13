//
//  ZNullInput.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZInput.hpp"
#include "ZCommon.hpp"

// Forward Declarations
class ZInputComponent;

// Class and Data Structure Definitions
class ZNullInput : public ZInput {
private:

public:
    virtual void Process() {
      _Z("No ZInput implementation has been provided", ZERROR);
    }
    virtual void GetPointerPosition(double& x, double& y) {
      _Z("No ZInput implementation has been provided", ZERROR);
    }
protected:


};
