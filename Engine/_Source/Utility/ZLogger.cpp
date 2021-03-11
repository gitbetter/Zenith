/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZLogger.cpp

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

#include "ZLogger.hpp"

void ZLogger::Log(const std::string& text, ZSeverity severity, const std::string& file, unsigned int line)
{
    ZLogEntry entry;
    entry.file = file;
    entry.line = line;
    entry.severity = severity;
    entry.text = text;
    AddEntry(entry);

#ifdef DEV_BUILD
    switch (entry.severity)
    {
    case ZSeverity::Info:
        std::cout << "\033[1;97m" << "[Info]: " << entry.file << ":" << entry.line << ": " << entry.text << "\033[0m" << std::endl;
        break;
    case ZSeverity::Warning:
        std::cout << "\033[1;33m" << "[Warning]: " << entry.file << ":" << entry.line << ": " << entry.text << "\033[0m" << std::endl;
        break;
    case ZSeverity::Error:
        std::cout << "\033[1;91m" << "[Error]: " << entry.file << ":" << entry.line << ": " << entry.text << "\033[0m" << std::endl;
        break;
    }
#endif
}

void ZLogger::AddEntry(const ZLogEntry& entry)
{
    std::lock_guard<std::mutex> lockBuffer(loggerMutexes_.buffer);
    logBuffer_.emplace_back(entry);
}
