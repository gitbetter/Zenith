/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZAnimation.hpp

		Created by Adrian Sanchez on 27/04/2019.
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

// Includes
#include "ZEngine.hpp"

// Forward Declarations
class ZModel;

// Class and Data Structure Definitions
template<class T>
struct ZAnimationKey {
	double time;
	T value;
};

struct ZJointAnimation {
	std::string jointName;
	std::vector<ZAnimationKey<glm::vec3>> scalingKeys;
	std::vector<ZAnimationKey<glm::quat>> rotationKeys;
	std::vector<ZAnimationKey<glm::vec3>> positionKeys;
};

struct ZAnimation {
	std::string name;
	double ticksPerSecond;
	double duration;
	std::vector<std::shared_ptr<ZJointAnimation>> channels;
};

struct ZAnimationClip {
	std::string name;
	double startTime;
	double endTime;
	double currentTime;
	std::shared_ptr<ZModel> model;
	ZAnimationState state;
};

