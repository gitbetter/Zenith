/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIScrubber.hpp

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
#include "ZUIClicker.hpp"
#include "ZServices.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZUIScrubber : public ZUIClicker
{

public:

    ZUIScrubber() : ZUIClicker()  { }
    ~ZUIScrubber() { }

    template<typename T>
    T Scrub(const ZRect& rect) {
        auto cursorXPos = ZServices::Input()->GetCursorPosition().x;
        if (!scrubbing_ && Clicked(rect)) {
            currentXPos_ = cursorXPos;
            scrubbing_ = true;
        }
        if (scrubbing_ && Released(rect)) {
            scrubbing_ = false;
        }
        return scrubbing_ ? sensitivity_ * static_cast<T>(cursorXPos - currentXPos_) : 0;
    }

    void SetSensitivity(float sensitivity) { sensitivity_ = sensitivity; }

protected:

    unsigned int currentXPos_ = 0;
    bool scrubbing_ = false;
    float sensitivity_ = 1.f;

};
