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

#include "ZUIClicker.hpp"
#include "ZServices.hpp"

bool ZUIClicker::Clicked(const ZRect& rect)
{
    if (activated_) return false;

    bool previouslyActivated = activated_;
    activated_ = Pressed(rect);
    return !previouslyActivated && activated_;
}

bool ZUIClicker::Pressed(const ZRect& rect, bool inRect)
{
    auto buttonDown = ZServices::Input()->Mouse(button_);
    auto cursorPos = ZServices::Input()->GetCursorPosition();
    if (activated_ && wrapToBounds_) {
        if (cursorPos.x < rect.position.x)
            cursorPos.x = rect.position.x + rect.size.x;
        if (cursorPos.x > rect.position.x + rect.size.x)
            cursorPos.x = rect.position.x;
        if (cursorPos.y < rect.position.y)
            cursorPos.y = rect.position.y + rect.size.y;
        if (cursorPos.y > rect.position.y + rect.size.y)
            cursorPos.y = rect.position.y;
        ZServices::Input()->SetCursorPosition(cursorPos);
    }
    return (inRect ? rect.Contains(cursorPos) : true) && buttonDown;
}

bool ZUIClicker::Released(const ZRect& rect)
{
    if (!activated_) return false;

    bool previouslyActivated = activated_;
    activated_ = Pressed(rect, false);
    return previouslyActivated && !activated_;
}