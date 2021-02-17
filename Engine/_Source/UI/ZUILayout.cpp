/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUILayout.cpp

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

#include "ZUILayout.hpp"
#include "ZUIElement.hpp"

ZUILayout::ZUILayout(ZRect dimensions, float itemSpacing, glm::vec2 itemSize, ZAlignment hAlign, ZAlignment vAlign)
{
    options_.dimensions = dimensions;
    options_.itemSpacing = itemSpacing;
    options_.defaultItemSize = itemSize;
    options_.horizontalAlign = hAlign;
    options_.verticalAlign = vAlign;
}

ZRect ZUILayout::GetRect(const std::string& element, const glm::vec2& size, bool force)
{
    auto it = std::find_if(calculatedRects_.begin(), calculatedRects_.end(),
        [&element](const ZUICalculatedLayoutEntry& entry) { return entry.id == element; });
    if (it != calculatedRects_.end()) {
        ZRect rect = (*it).rect;
        if (rect.size != size || force) {
            rect.size = size;
            (*it).rect = rect;
            RecalculateRects(it);
            it = std::find_if(calculatedRects_.begin(), calculatedRects_.end(),
                [&element](const ZUICalculatedLayoutEntry& entry) { return entry.id == element; });
        }
        return (*it).rect;
    }

    ZUICalculatedLayoutEntry newEntry;
    newEntry.id = element;
    newEntry.rect = CalculateNewRect(element, size == glm::vec2(0.f) ? options_.defaultItemSize : size);
    calculatedRects_.push_back(newEntry);

    return newEntry.rect;
}

void ZUILayout::RemoveRect(const std::string& element)
{
    auto it = std::find_if(calculatedRects_.begin(), calculatedRects_.end(),
        [&element](const ZUICalculatedLayoutEntry& entry) { return entry.id == element; });
    if (it == calculatedRects_.end())
        return;

    it = calculatedRects_.erase(it);

    RecalculateRects(it);
}

void ZUILayout::SetDimensions(const ZRect& rect)
{
    options_.dimensions = rect;
}

void ZUILayout::RecalculateRects(const std::vector<ZUICalculatedLayoutEntry>::iterator& start)
{
    std::vector<ZUICalculatedLayoutEntry> oldCalculatedRects(calculatedRects_);
    auto it = std::find_if(oldCalculatedRects.begin(), oldCalculatedRects.end(),
        [&start](const ZUICalculatedLayoutEntry& entry) { return entry.id == start->id; });
    calculatedRects_ = std::vector<ZUICalculatedLayoutEntry>(calculatedRects_.begin(), start);

    for (; it != oldCalculatedRects.end(); it++)
    {
        ZUICalculatedLayoutEntry entry = *it;
        entry.rect = CalculateNewRect(entry.id, entry.rect.size);
        calculatedRects_.push_back(entry);
    }
}