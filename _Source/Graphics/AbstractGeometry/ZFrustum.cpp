/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZFrustum.cpp

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

#include "ZFrustum.hpp"

ZFrustum::ZFrustum(float fov, float ratio, float near, float far) {
	this->fov = fov;
	this->ratio = ratio;
	this->near = near;
	this->far = far;

	tang = (float)glm::tan(glm::radians(fov) * 0.5f);
	nearHeight = near * tang;
	nearWidth = nearHeight * ratio;
	farHeight = far * tang;
	farWidth = farHeight * ratio;
}

void ZFrustum::Recalculate(glm::vec3 pos, glm::vec3 front, glm::vec3 up) {
	glm::vec3 dir, nearCenter, farCenter, x, y, z;

	z = glm::normalize(pos - front);
	x = glm::normalize(glm::cross(up, z));
	y = glm::cross(z, x);

	nearCenter = pos - z * near;
	farCenter = pos - z * far;

	planes[NEAR] = ZPlane(-z, nearCenter);
	planes[FAR] = ZPlane(z, farCenter);

	glm::vec3 point, normal;
	point = nearCenter + y * nearHeight;
	normal = glm::cross(glm::normalize(point - pos), x);
	planes[TOP] = ZPlane(normal, point);

	point = nearCenter - y * nearHeight;
	normal = glm::cross(x, glm::normalize(point - pos));
	planes[BOTTOM] = ZPlane(normal, point);

	point = nearCenter - x * nearWidth;
	normal = glm::cross(glm::normalize(point - pos), y);
	planes[LEFT] = ZPlane(normal, point);

	point = nearCenter + x * nearWidth;
	normal = glm::cross(y, glm::normalize(point - pos));
	planes[RIGHT] = ZPlane(normal, point);
}

bool ZFrustum::Contains(glm::vec3 point) {
	for (int plane = 0; plane < NUMPLANES; plane++) {
		if (planes[plane].Distance(point) < 0.f)
			return false;
	}
	return true;
}

bool ZFrustum::Contains(glm::vec3 center, float radius) {
	float distance;
	for (int plane = 0; plane < NUMPLANES; plane++) {
		distance = planes[plane].Distance(center);
		if (distance < -radius) {
			return false;
		}
	}
	return true;
}

bool ZFrustum::Contains(const ZAABBox& box) {
	for (int plane = 0; plane < NUMPLANES; plane++) {
		if (planes[plane].Distance(box.maximum) < 0 &&
				planes[plane].Distance(box.minimum) < 0) {
			return false;
		}
	}
	return true;
}