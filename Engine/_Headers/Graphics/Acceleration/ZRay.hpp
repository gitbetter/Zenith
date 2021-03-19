/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZRay.hpp

        Created by Adrian Sanchez on 02/23/2021.
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

// Class and Data Structure Definitions
class ZRay
{

public:

    float tMax = std::numeric_limits<float>::max();

    ZRay(const glm::vec3& origin, const glm::vec3& direction) 
        : origin_(origin), direction_(direction)
    { }

    const glm::vec3 Position(float t) const noexcept
    {
        return origin_ + direction_ * t;
    }

    float ClosestDistanceTo(const ZRay& other);
    glm::vec3 ClosestPointTo(const ZRay& other);

    const glm::vec3& Origin() const { return origin_; }
    const glm::vec3& Direction() const { return direction_; }

private:

    glm::vec3 origin_;
    glm::vec3 direction_;

};

inline const ZRay operator*(const glm::mat4& mat, const ZRay& r) noexcept
{
    return ZRay(mat * glm::vec4(r.Origin(), 1.f), mat * glm::vec4(r.Direction(), 1.f));
}

