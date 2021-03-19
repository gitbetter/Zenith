/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZAABBox.cpp

        Created by Adrian Sanchez on 18/04/2019.
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

#include "ZAABBox.hpp"

bool ZAABBox::Intersects(ZRay& ray)
{
    auto check_axis = [](float origin, float direction, float min, float max) {
        auto tmin_numerator = min - origin;
        auto tmax_numerator = max - origin;

        float tmin, tmax;
        if (abs(direction) >= EPSILON)
        {
            tmin = tmin_numerator / direction;
            tmax = tmax_numerator / direction;
        }
        else
        {
            tmin = tmin_numerator * std::numeric_limits<float>::infinity();
            tmax = tmax_numerator * std::numeric_limits<float>::infinity();
        }

        return tmin > tmax ? std::array<float, 2>{tmax, tmin} : std::array<float, 2>{tmin, tmax};
    };

    auto origin = ray.Origin();
    auto direction = ray.Direction();
    auto xt = check_axis(origin.x, direction.x, minimum.x, maximum.x);
    auto yt = check_axis(origin.y, direction.y, minimum.y, maximum.y);
    auto zt = check_axis(origin.z, direction.z, minimum.z, maximum.z);

    auto tmin = std::max({ xt[0], yt[0], zt[0] });
    auto tmax = std::min({ xt[1], yt[1], zt[1] });

    ray.tMax = tmin;

    return tmin <= tmax;
}

glm::vec3 ZAABBox::Offset(const glm::vec3& p) const
{
    glm::vec3 o = p - minimum;
    if (maximum.x > minimum.x) o.x /= maximum.x - minimum.x;
    if (maximum.y > minimum.y) o.y /= maximum.y - minimum.y;
    if (maximum.z > minimum.z) o.z /= maximum.z - minimum.z;
    return o;
}

int ZAABBox::MaxExtent() const
{
    glm::vec3 diag = Diagonal();
    if (diag.x > diag.y && diag.x > diag.z)
        return 0;
    else if (diag.y > diag.z)
        return 1;
    return 2;
}

float ZAABBox::SurfaceArea() const
{
    glm::vec3 diag = Diagonal();
    return 2.f * (diag.x * diag.y + diag.x * diag.z + diag.y * diag.z);
}
