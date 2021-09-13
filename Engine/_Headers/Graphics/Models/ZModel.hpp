/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZModel.hpp

    Created by Adrian Sanchez on 1/25/19.
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

#include "ZMesh.hpp"
#include "ZTexture.hpp"
#include "ZAABBox.hpp"

struct ZSkeleton;
struct ZJoint;
struct ZJointAnimation;
class ZResourceLoadedEvent;
class ZUniformBuffer;
class ZRenderStateGroup;

struct ZModel
{
	std::string name;
	std::string modelPath;
	ZMesh3DMap meshes;
	ZBoneMap bonesMap;
	ZBoneList bones;
	ZAnimationMap animations;
	ZSkeleton skeleton;
	ZInstancedDataOptions instanceData;
	glm::mat4 globalInverseTransform;
	ZAABBox bounds;

	std::shared_ptr<ZRenderStateGroup> renderState;
	std::shared_ptr<ZUniformBuffer> uniformBuffer;

private:

	static ZIDSequence idGenerator_;
};

class ZModelManager
{

	using ZModelPool = ZResourcePool<ZModel, ZHModel>;

public:

    ZModelManager() = default;
	~ZModelManager() = default;

    ZHModel Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
    void DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
    std::shared_ptr<ZModel> Create(const std::string& type, const std::shared_ptr<ZOFNode>& data = nullptr);
    std::shared_ptr<ZModel> CreateExternal(const std::string& path, bool async = false);

    virtual void Initialize();
    virtual void Initialize(const std::shared_ptr<ZOFNode>& data);
    virtual void InitializeAsync();

	bool IsLoaded(const std::string& name);
    ZHModel GetFromName(const std::string& name);

    const std::string& ID() const { return id_; }
    const std::string& Path() const { return modelPath_; }
    const ZMesh3DMap& Meshes() const { return meshes_; }
    const ZBoneList& Bones() const { return bones_; }
    const ZBoneMap& BonesMap() const { return bonesMap_; }
    const ZAnimationMap& Animations() const { return animations_; }
    const ZInstancedDataOptions& InstanceData() const { return instanceData_; }
    const std::shared_ptr<ZSkeleton> Skeleton() const { return skeleton_; }
    const glm::mat4 GlobalInverseTransform() const { return globalInverseTransform_; }
    const ZAABBox& Bounds() const { return bounds_; }
    const std::shared_ptr<ZRenderStateGroup> RenderState() const { return renderState_; }

    void SetInstanceData(const ZInstancedDataOptions& instanceData);
    void BoneTransform(const std::string& anim, double secondsTime);

protected:

    ZModelPool modelPool_;
	ZModelMap loadedModels_;

protected:

	/** Adds a shader to the internal loaded shader map so that we don't accidentally recreate duplicates of the shader */
	void Track(const ZHModel& handle);

    void ComputeBounds();
    void CalculateTransformsInHierarchy(const std::string& animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform);
    glm::vec3 CalculateInterpolatedScaling(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    glm::quat CalculateInterpolatedRotation(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    glm::vec3 CalculateInterpolatedPosition(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);

    void HandleModelLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);

};
