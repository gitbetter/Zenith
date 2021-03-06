/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZAbstractPlane.hpp

        Created by Adrian Sanchez on 18/04/2019.
        Copyright � 2019 Pervasive Sense. All rights reserved.

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
class ZAbstractPlane
{

public:

    glm::vec3 center;
    glm::vec3 normal;

    ZAbstractPlane() {}
    ZAbstractPlane(const glm::vec3& c, const glm::vec3& n) : center(c), normal(n) {}
    ZAbstractPlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

    float Distance(const glm::vec3& point);
    glm::vec3 Intersection(const ZAbstractPlane& a, const ZAbstractPlane& b);

};

