//
//  ZLogger.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <string>

// Forward Declarations
// class SomeClass;

// Class Definitions
enum ZLoggerSeverity {
    Error, Warning, Info
};

class ZLogger {
public:
    static void Log(std::string message, ZLoggerSeverity severity);
};

