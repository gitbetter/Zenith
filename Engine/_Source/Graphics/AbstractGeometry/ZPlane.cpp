/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZPlane.cpp

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

#include "ZPlane.hpp"

ZPlane::ZPlane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	glm::vec3 e2 = p2 - p1, e1 = p3 - p1;
	normal = glm::normalize(glm::cross(e2, e1));
	center = normal * glm::dot(p1, normal);
}

float ZPlane::Distance(glm::vec3 point) {
	return glm::length(glm::dot(point, glm::normalize(normal)) - center);
}

glm::vec3 ZPlane::Intersection(ZPlane a, ZPlane b) {
    float f = -glm::dot(normal, glm::cross(a.normal, b.normal));
    
    glm::vec3 v1(center * glm::cross(a.normal, b.normal));
    glm::vec3 v2(a.center * glm::cross(b.normal, normal));
    glm::vec3 v3(b.center * glm::cross(normal, a.normal));
    
    return (v1 + v2 + v3) / f;
}
