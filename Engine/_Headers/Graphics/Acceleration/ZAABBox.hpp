/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZAABBox.hpp

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

#include "ZRay.hpp"

class ZAABBox
{

public:

    glm::vec3 minimum, maximum;

    ZAABBox() : minimum(std::numeric_limits<float>::max()), maximum(std::numeric_limits<float>::min()) {}
    ZAABBox(const glm::vec3& min, const glm::vec3& max) : minimum(min), maximum(max) {}

    bool Intersects(ZRay& ray);
    glm::vec3 Offset(const glm::vec3& p) const;
    int MaxExtent() const;
    float SurfaceArea() const;

    glm::vec3 Diagonal() const { return maximum - minimum; }

	const bool inline IsValid() const
    {
		static const float max = std::numeric_limits<float>::max();
        static const float min = std::numeric_limits<float>::min();
		return minimum.x < max&& minimum.y < max&& minimum.z < max
			&& maximum.x > min&& maximum.y > min&& maximum.z > min;
	}

    static const ZAABBox Union(const ZAABBox& b, const glm::vec3& p) noexcept
    {
        return ZAABBox(glm::vec3(std::min(b.minimum.x, p.x),
                                 std::min(b.minimum.y, p.y),
                                 std::min(b.minimum.z, p.z)),
                       glm::vec3(std::max(b.maximum.x, p.x),
                                 std::max(b.maximum.y, p.y),
                                 std::max(b.maximum.z, p.z)));
    }

    static const ZAABBox Union(const ZAABBox& b1, const ZAABBox& b2) noexcept
    {
        return ZAABBox(glm::vec3(std::min(b1.minimum.x, b2.minimum.x),
            std::min(b1.minimum.y, b2.minimum.y),
            std::min(b1.minimum.z, b2.minimum.z)),
            glm::vec3(std::max(b1.maximum.x, b2.maximum.x),
                std::max(b1.maximum.y, b2.maximum.y),
                std::max(b1.maximum.z, b2.maximum.z)));
    }
};

inline const ZAABBox operator*(const glm::mat4& mat, const ZAABBox& box) noexcept
{
    ZAABBox result(mat[3], mat[3]);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float a = mat[j][i] * box.minimum[j];
            float b = mat[j][i] * box.maximum[j];
            result.minimum[i] += a < b ? a : b;
            result.maximum[i] += a < b ? b : a;
        }
    }
    return result;
}