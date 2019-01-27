//
//  ZLogger.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZLogger.hpp"
#include <iostream>

void ZLogger::Log(std::string message, ZLoggerSeverity severity) {
    switch (severity) {
        case ZLoggerSeverity::Error:
            std::cout << "\033[1;91m" << "ZLogger [Error]: " << message << "\033[0m" << std::endl;
            break;
        case ZLoggerSeverity::Warning:
            std::cout << "\033[1;33m" << "ZLogger [Warning]: " << message << "\033[0m" << std::endl;
            break;
        case ZLoggerSeverity::Info:
            std::cout << "\033[1;97m" << "ZLogger [Info]: " << message << "\033[0m" << std::endl;
            break;
    }
}
