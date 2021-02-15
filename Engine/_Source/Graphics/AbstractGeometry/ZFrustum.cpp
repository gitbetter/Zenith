/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZFrustum.cpp

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

#include "ZFrustum.hpp"

ZFrustum::ZFrustum(float fov, float ratio, float near, float far)
{
    this->fov = fov;
    this->ratio = ratio;
    this->near = near;
    this->far = far;

    SetBasis(glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f));
}

void ZFrustum::SetBasis(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up)
{
    position = pos;
    basis.z = glm::normalize(pos - front);
    basis.x = glm::normalize(glm::cross(up, basis.z));
    basis.y = glm::normalize(glm::cross(basis.z, basis.x));
    Recalculate();
}

void ZFrustum::Recalculate()
{
    tang = (float)glm::tan(glm::radians(fov * 0.5f));
    nearHeight = near * tang;
    nearWidth = nearHeight * ratio;
    farHeight = far * tang;
    farWidth = farHeight * ratio;

    nearCenter = position - basis.z * near;
    farCenter = position - basis.z * far;

    corners[0] = nearCenter + basis.y * nearHeight - basis.x * nearWidth; // Near Top Left
    corners[1] = nearCenter + basis.y * nearHeight + basis.x * nearWidth; // Near Top Right
    corners[2] = nearCenter - basis.y * nearHeight + basis.x * nearWidth; // Near Bottom Right
    corners[3] = nearCenter - basis.y * nearHeight - basis.x * nearWidth; // Near Bottom Left
    corners[4] = farCenter + basis.y * farHeight - basis.x * farWidth;	  // Far Top Left
    corners[5] = farCenter + basis.y * farHeight + basis.x * farWidth;	  // Far Top Right
    corners[6] = farCenter - basis.y * farHeight + basis.x * farWidth;	  // Far Bottom Right
    corners[7] = farCenter - basis.y * farHeight - basis.x * farWidth;	  // Far Bottom Left

    center = glm::vec3(0.f);
    for (const auto& corner : corners) {
        center += corner;
    }
    center /= 8;

    planes[NEAR] = ZPlane(corners[0], corners[0], corners[2]);
    planes[FAR] = ZPlane(corners[5], corners[4], corners[7]);
    planes[TOP] = ZPlane(corners[1], corners[0], corners[4]);
    planes[BOTTOM] = ZPlane(corners[3], corners[2], corners[6]);
    planes[LEFT] = ZPlane(corners[0], corners[3], corners[7]);
    planes[RIGHT] = ZPlane(corners[2], corners[1], corners[6]);
}

bool ZFrustum::Contains(const glm::vec3& point)
{
    for (int plane = 0; plane < NUMPLANES; plane++)
    {
        if (planes[plane].Distance(point) < 0.f)
            return false;
    }
    return true;
}

bool ZFrustum::Contains(const glm::vec3& center, float radius)
{
    float distance;
    for (int plane = 0; plane < NUMPLANES; plane++)
    {
        distance = planes[plane].Distance(center);
        if (distance < -radius)
        {
            return false;
        }
    }
    return true;
}

bool ZFrustum::Contains(const ZAABBox& box)
{
    for (int plane = 0; plane < NUMPLANES; plane++)
    {
        if (planes[plane].Distance(box.maximum) < 0 &&
            planes[plane].Distance(box.minimum) < 0)
        {
            return false;
        }
    }
    return true;
}

bool ZFrustum::Contains(const ZFrustum& frustum)
{
// TODO:
    return true;
}
