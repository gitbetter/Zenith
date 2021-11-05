/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIHelpers.hpp

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

#include "ZCommon.hpp"

class ZUIHelpers {

public:

    static glm::vec2 RelativeToAbsoluteCoords(const glm::vec2& coords, const glm::vec2& parentCoords = glm::vec2(0.f));
    static ZRect RelativeToAbsoluteRect(const ZRect& rect, const ZRect& parentRect = ZRect());

    static glm::vec2 AbsoluteToRelativeCoords(const glm::vec2& coords, const glm::vec2& parentCoords = glm::vec2(0.f));
    static ZRect AbsoluteToRelativeRect(const ZRect& rect, const ZRect& parentRect = ZRect());

	static struct ZMesh2D NewQuad();
	static struct ZMesh2D NewScreenTriangle();
};