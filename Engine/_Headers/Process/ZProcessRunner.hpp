/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZProcessRunner.hpp

    Created by Adrian Sanchez on 23/03/2019.
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
#include "ZEngine.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZProcessRunner
{

private:

    void ClearProcesses();

public:

    ~ZProcessRunner();

    void UpdateTick();
    void UpdateProcesses(ZPriority priority);
    void AttachProcess(std::shared_ptr<ZProcess> process, ZPriority priority = ZPriority::FirstPriority);
    void AbortAllProcesses(bool immediate);

    unsigned int ProcessCount(ZPriority priority) const { 
        assert(priority >= ZPriority::FirstPriority && priority < ZPriority::LastPriority); 
        return processList_[priority].size();
    }

protected:

    ZProcessList processList_[ZPriority::LastPriority];

};
