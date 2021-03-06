/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZFrustum.hpp

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
#include "ZAbstractPlane.hpp"
#include "ZAABBox.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZFrustum
{

public:

    enum { TOP, BOTTOM, LEFT, RIGHT, NEAR, FAR, NUMPLANES };

    ZBasis basis;
    ZAbstractPlane planes[NUMPLANES];
    glm::vec3 corners[8];
    glm::vec3 nearCenter, farCenter, center;
    glm::vec3 position;
    float near, far, ratio, fov, tang;
    float nearWidth, nearHeight, farWidth, farHeight;

    ZFrustum() {}
    ZFrustum(float fov, float ratio, float near, float far);

    void SetBasis(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up);

    void Recalculate();

    bool Contains(const glm::vec3& point);
    bool Contains(const glm::vec3& center, float radius);
    bool Contains(const ZAABBox& box);
    bool Contains(const ZFrustum& frustum);

};

