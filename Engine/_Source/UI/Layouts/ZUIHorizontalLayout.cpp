/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIHorizontalLayout.cpp

    Created by Adrian Sanchez on 20/01/2021.
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

#include "ZUIHorizontalLayout.hpp"

ZRect ZUIHorizontalLayout::CalculateNewRect(const std::string& element, const glm::vec2& size)
{
    ZRect newRect;
    newRect.size = size;
    newRect.position.y = options_.dimensions.position.y;
    switch (options_.verticalAlign) {
    case ZAlignment::Middle:
        newRect.position.y += glm::clamp(options_.dimensions.size.y - size.y, 0.f, options_.dimensions.size.y) / 2.f;
        break;
    case ZAlignment::Bottom:
        newRect.position.y += glm::clamp(options_.dimensions.size.y - size.y, 0.f, options_.dimensions.size.y);
        break;
    default:
        break;
    }
    if (!calculatedRects_.empty()) {
        ZUICalculatedLayoutEntry lastEntry = calculatedRects_.back();
        newRect.position.x = lastEntry.rect.position.x + lastEntry.rect.size.x + options_.itemSpacing;
    }
    else {
        newRect.position.x = options_.dimensions.position.x;
    }

    return newRect;
}