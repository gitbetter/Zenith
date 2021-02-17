/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUILayout.hpp

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

#pragma once

// Includes
#include "ZCommon.hpp"

// Class and Data Structure Definitions

struct ZUILayoutOptions
{
    ZRect dimensions;
    float itemSpacing{ 0.f };
    glm::vec2 defaultItemSize{ 0.f, 0.f };
    ZAlignment verticalAlign{ ZAlignment::Top };
    ZAlignment horizontalAlign{ ZAlignment::Left };
};

struct ZUICalculatedLayoutEntry
{
    std::string id;
    ZRect rect;
};

class ZUILayout
{

public:

    ZUILayout() { }
    ZUILayout(const ZUILayoutOptions& options) : options_(options) { }
    ZUILayout(ZRect dimensions, float itemSpacing, glm::vec2 itemSize = glm::vec2(0.f),
        ZAlignment hAlign = ZAlignment::Left, ZAlignment vAlign = ZAlignment::Top);

    virtual ~ZUILayout() { }

    void SetDimensions(const ZRect& rect);

    ZRect GetRect(const std::string& element, const glm::vec2& size, bool force = false);
    void RemoveRect(const std::string& element);

protected:

    void RecalculateRects(const std::vector<ZUICalculatedLayoutEntry>::iterator& start);

    virtual ZRect CalculateNewRect(const std::string& element, const glm::vec2& size) = 0;

protected:

    ZUILayoutOptions options_;
    std::vector<ZUICalculatedLayoutEntry> calculatedRects_;

};
