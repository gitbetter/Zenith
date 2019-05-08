/*
 
 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
 ZMesh.hpp
 
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

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZProcess.hpp"

// Forward Declarations
class ZModel;
class ZShader;
struct ZSkeleton;
struct ZJoint;
struct ZAnimation;
struct ZJointAnimation;

// Class and Data Structure Definitions
class ZMesh : public ZProcess {
    
    friend class ZModel;
    
public:
    
    virtual ~ZMesh() { }
    
    virtual void Render(ZShader* shader, ZMaterial* material) = 0;
    virtual ZMeshDrawStyle DrawStyle() const { return drawStyle_; };
	std::shared_ptr<ZSkeleton> Skeleton() { return skeleton_; }
    ZAnimationMap& Animations() { return animations_; }
    
	virtual void SetSkeleton(std::shared_ptr<ZSkeleton> skeleton);
    virtual void SetAnimations(ZAnimationMap animations);

    void BoneTransform(std::string anim, float secondsTime);
    
protected:
    
    ZBufferData bufferData_;
    ZMeshDrawStyle drawStyle_;

	glm::mat4 globalInverseTransform_;

	std::shared_ptr<ZSkeleton> skeleton_;
	ZAnimationMap animations_;
    ZModel* model_ = nullptr;

	void CalculateTransformsInHierarchy(std::string animName, float animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform);
	glm::vec3 CalculateInterpolatedScaling(float animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
	glm::quat CalculateInterpolatedRotation(float animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
	glm::vec3 CalculateInterpolatedPosition(float animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    
};
