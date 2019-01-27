//
//  ZNullGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphics.hpp"

// Forward Declarations
class SomeClass;

// Class and Data Structure Definitions
class ZNullGraphics : public ZGraphics {
private:

public:
    ZNullGraphics() {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }
    ~ZNullGraphics() { }

    void Draw() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }
    void Delete() override {
      ZLogger::Log("No ZGraphics implementation has been provided.", ZLoggerSeverity::Error);
    }
protected:

};
