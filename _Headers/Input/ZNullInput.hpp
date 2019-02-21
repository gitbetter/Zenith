//
//  ZNullInput.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
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

    virtual void Register(ZObject* gameObject) {
      _Z("No ZInput implementation has been provided", ZERROR);
    }
    virtual void Broadcast(const ZInputCommand& command) {
      _Z("No ZInput implementation has been provided", ZERROR);
    }
    virtual void Process() {
      _Z("No ZInput implementation has been provided", ZERROR);
    }
protected:


};
