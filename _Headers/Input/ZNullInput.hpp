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
#include "ZLogger.hpp"

// Forward Declarations
class ZInputComponent;

// Class and Data Structure Definitions
class ZNullInput : public ZInput {
private:
public:
    virtual void Register(ZObject& gameObject) {
      ZLogger::Log("No ZInput implementation has been provided", ZLoggerSeverity::Error);
    }
    virtual void Broadcast(const ZObject& command) {
      ZLogger::Log("No ZInput implementation has been provided", ZLoggerSeverity::Error);
    }
    virtual void Process() {
      ZLogger::Log("No ZInput implementation has been provided", ZLoggerSeverity::Error);
    }
protected:

};
