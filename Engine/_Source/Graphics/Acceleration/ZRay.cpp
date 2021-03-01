/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZRay.cpp

        Created by Adrian Sanchez on 02/23/2021.
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

#include "ZRay.hpp"

float ZRay::ClosestDistanceTo(const ZRay& other)
{
    const glm::vec3 dp = other.origin_ - origin_;
    const float v12 = glm::dot(direction_, direction_);
    const float v22 = glm::dot(other.direction_, other.direction_);
    const float v1v2 = glm::dot(direction_, other.direction_);

    const float det = v1v2 * v1v2 - v12 * v22;

    if (std::abs(det) > EPSILON) {
        const float inv_det = 1.f / det;

        const float dpv1 = glm::dot(dp, direction_);
        const float dpv2 = glm::dot(dp, other.direction_);

        float t1 = inv_det * (v22 * dpv1 - v1v2 * dpv2);
        float t2 = inv_det * (v1v2 * dpv1 - v12 * dpv2);

        return glm::length(dp + other.direction_ * t2 - direction_ * t1);
    }
    else {
        const glm::vec3 a = glm::cross(dp, direction_);
        return std::sqrt(glm::dot(a, a) / v12);
    }
}

glm::vec3 ZRay::ClosestPointTo(const ZRay& other)
{
    const glm::vec3 dp = other.origin_ - origin_;
    const float v12 = glm::dot(direction_, direction_);
    const float v22 = glm::dot(other.direction_, other.direction_);
    const float v1v2 = glm::dot(direction_, other.direction_);

    const float det = v1v2 * v1v2 - v12 * v22;

    if (std::abs(det) > EPSILON) {
        const float inv_det = 1.f / det;

        const float dpv1 = glm::dot(dp, direction_);
        const float dpv2 = glm::dot(dp, other.direction_);

        float t = inv_det * (v22 * dpv1 - v1v2 * dpv2);
        return Position(t);
    }

    return Position(std::numeric_limits<float>::infinity());
}
