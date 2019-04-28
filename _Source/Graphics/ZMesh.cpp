/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZMesh.cpp

    Created by Adrian Sanchez on 06/02/2019.
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

#include "ZMesh.hpp"
#include "ZAnimation.hpp"
#include "ZSkeleton.hpp"
#include <assimp/scene.h>

void ZVertex3D::AddBoneData(unsigned int bone, float weight) {
    for (unsigned int i = 0; i < BONES_PER_VERTEX; i++) {
        if (boneWeights[i] == 0.f) {
            boneIDs[i] = bone;
            boneWeights[i] = weight;
            return;
        }
    }
}

std::vector<glm::mat4> ZMesh::BoneTransform(float secondsTime) {
    glm::mat4 identity(1.f);
    std::vector<glm::mat4> transforms;
    
    // TODO: Parametrize animation index
    float ticksPerSecond = animations_[0]->ticksPerSecond != 0 ? (float)animations_[0]->ticksPerSecond : 25.f;
    float timeInTicks = secondsTime * ticksPerSecond;
    float animationTime = fmod(timeInTicks, animations_[0]->duration);
    
	CalculateTransformsInHierarchy(animationTime, skeleton_->rootJoint, identity);
    
	for (ZBoneMap::iterator it = skeleton_->bones.begin(), end = skeleton_->bones.end(); it != end; it++) {
        transforms.push_back(it->second->transformation);
    }
    
    return transforms;
}

void ZMesh::CalculateTransformsInHierarchy(float animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform) {
    std::shared_ptr<ZAnimation> animation = animations_[0];
	glm::mat4 jointTransform = joint->transform;
    
    std::shared_ptr<ZJointAnimation> jointAnimation;
    for (unsigned int i = 0, j = animation->channels.size(); i < j; i++) {
		std::shared_ptr<ZJointAnimation> anim = animation->channels[i];
        if (anim->jointName == joint->name) {
            jointAnimation = anim;
        }
    }
    
    if (jointAnimation) {
		glm::vec3 scale = CalculateInterpolatedScaling(animTime, jointAnimation);
		glm::quat rotation = CalculateInterpolatedRotation(animTime, jointAnimation);
		glm::vec3 position = CalculateInterpolatedPosition(animTime, jointAnimation);

		jointTransform = glm::mat4_cast(rotation);
		jointTransform = glm::scale(jointTransform, scale);
		jointTransform = glm::translate(jointTransform, position);
    }

	glm::mat4 globalTransform = parentTransform * jointTransform;

	if (skeleton_->bones.find(joint->name) != skeleton_->bones.end()) {
		skeleton_->bones[joint->name]->transformation = globalInverseTransform_ * globalTransform * skeleton_->bones[joint->name]->offset;
	}

	for (unsigned int i = 0; i < joint->children.size(); i++) {
		CalculateTransformsInHierarchy(animTime, joint->children[i], globalTransform);
	}
}

glm::vec3 ZMesh::CalculateInterpolatedScaling(float animationTime, std::shared_ptr<ZJointAnimation> jointAnim) {
	if (jointAnim->scalingKeys.size() == 1) {
		return jointAnim->scalingKeys[0].value;
	}

	unsigned int scalingIndex = 0;
	for (unsigned int i = 0, j = jointAnim->scalingKeys.size() - 1; i < j; i++) {
		if (animationTime < (float)jointAnim->scalingKeys[i + 1].time)
			scalingIndex = i;
	}
	unsigned int nextScalingIndex = scalingIndex + 1;
	float deltaTime = (float)(jointAnim->scalingKeys[nextScalingIndex].time - jointAnim->scalingKeys[scalingIndex].time);
	float factor = (animationTime - (float)jointAnim->scalingKeys[scalingIndex].time) / deltaTime;
	glm::vec3 start = jointAnim->scalingKeys[scalingIndex].value;
	glm::vec3 end = jointAnim->scalingKeys[nextScalingIndex].value;
	glm::vec3 delta = end - start;

	return start + factor * delta;
}

glm::quat ZMesh::CalculateInterpolatedRotation(float animationTime, std::shared_ptr<ZJointAnimation> jointAnim) {
	if (jointAnim->rotationKeys.size() == 1) {
		return jointAnim->rotationKeys[0].value;
	}

	unsigned int rotationIndex = 0;
	for (unsigned int i = 0, j = jointAnim->rotationKeys.size(); i < j; i++) {
		if (animationTime < (float)jointAnim->rotationKeys[i + 1].time)
			rotationIndex = i;
	}
	unsigned int nextRotationIndex = rotationIndex + 1;
	float deltaTime = (float)(jointAnim->rotationKeys[nextRotationIndex].time - jointAnim->rotationKeys[rotationIndex].time);
	float factor = (animationTime - (float)jointAnim->rotationKeys[rotationIndex].time) / deltaTime;
	glm::quat start = jointAnim->rotationKeys[rotationIndex].value;
	glm::quat end = jointAnim->rotationKeys[nextRotationIndex].value;
	glm::quat out = glm::mix(start, end, factor);

	return glm::normalize(out);
}

glm::vec3 ZMesh::CalculateInterpolatedPosition(float animationTime, std::shared_ptr<ZJointAnimation> jointAnim) {
	if (jointAnim->positionKeys.size() == 1) {
		return jointAnim->positionKeys[0].value;
	}

	unsigned int positionIndex = 0;
	for (unsigned int i = 0, j = jointAnim->rotationKeys.size() - 1; i < j; i++) {
		if (animationTime < (float)jointAnim->positionKeys[i + 1].time)
			positionIndex = i;
	}
	unsigned int nextPositionIndex = positionIndex + 1;
	float deltaTime = (float)(jointAnim->positionKeys[nextPositionIndex].time - jointAnim->positionKeys[positionIndex].time);
	float factor = (animationTime - (float)jointAnim->positionKeys[positionIndex].time) / deltaTime;
	glm::vec3 start = jointAnim->positionKeys[positionIndex].value;
	glm::vec3 end = jointAnim->positionKeys[nextPositionIndex].value;
	glm::vec3 delta = end - start;

	return start + factor * delta;
}

void ZMesh::SetSkeleton(std::shared_ptr<ZSkeleton> skeleton) { 
	globalInverseTransform_ = glm::inverse(skeleton->rootJoint->transform);
	skeleton_ = skeleton; 
}