/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZTimedUpdateTask.hpp

    Created by Adrian Sanchez on 02/09/2020.
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
#include "ZProcess.hpp"
#include <functional>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZTimedUpdateTask : public ZProcess
{

private:

    float duration_;
    float elapsed_;
    ZTimedUpdateCallback callback_;

public:

    ZTimedUpdateTask(float duration, const ZTimedUpdateCallback& callback) :
        ZProcess(), duration_(duration), elapsed_(0.f), callback_(callback) {}
    ~ZTimedUpdateTask() {};

protected:

    void OnUpdate(double deltaTime) override;

};
