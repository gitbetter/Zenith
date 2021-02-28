/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIClicker.hpp

    Created by Adrian Sanchez on 02/16/2021.
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

// Class and Data Structure Definitions
class ZUIClicker
{

public:

    ZUIClicker(const ZMouse& button = ZMouse::LEFT_MB)
        : button_(button)
    { }
    ~ZUIClicker() { }

    bool Click(const ZRect& rect);
    bool Press(const ZRect& rect);
    bool Release(const ZRect& rect);

    void WrapToBounds(bool wrap = true) { wrapToBounds_ = wrap; }

protected:

    bool activated_ = false;
    bool buttonDown_ = false;
    bool wrapToBounds_ = false;
    ZMouse button_;

    bool Clicked(const ZRect& rect);
    bool Pressed(const ZRect& rect, bool inRect = true);
    bool Released(const ZRect& rect);

};
