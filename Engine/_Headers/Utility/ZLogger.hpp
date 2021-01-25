/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZLogger.hpp

    Created by Adrian Sanchez on 22/05/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

 This file is part of Zenith.

 Zenith is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zenith is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
enum class ZSeverity
{
    Info, Warning, Error
};

struct ZLogEntry
{
    ZSeverity severity;    
    std::string file;
    unsigned int line;
    std::string text;

    ZLogEntry()
        : severity(ZSeverity::Info), file(""), line(0), text("")
    { }
    ZLogEntry(const std::string& text, const std::string& file, unsigned int line, ZSeverity severity)
        : severity(severity), file(file), line(line), text(text)
    { }
};

namespace zenith {
    extern void Log(const std::string& text, ZSeverity severity);
}

class ZLogger
{

private:

    struct
    {
        std::mutex buffer;
    } loggerMutexes_;

    std::vector<ZLogEntry> logBuffer_;

    void AddEntry(const ZLogEntry& entry);

public:

    const std::vector<ZLogEntry>& Buffer() const { return logBuffer_; }

    void Log(const ZLogEntry& entry);
    void Clear() { logBuffer_.clear(); }

};
