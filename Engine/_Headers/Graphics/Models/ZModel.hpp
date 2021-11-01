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

#include "ZResourceManager.hpp"
#include "ZMesh.hpp"
#include "ZTexture.hpp"
#include "ZAABBox.hpp"

struct ZSkeleton;
struct ZJoint;
struct ZJointAnimation;
class ZResourceLoadedEvent;
class ZUniformBuffer;
class ZRenderStateGroup;

enum class ZModelType
{
    Plane, Cube, Sphere, Cylinder, Cone, Composite, Custom
};

struct ZModel
{
	std::string name;
	std::string path;
    ZMesh3DList meshes;
	ZBoneMap bonesMap;
	ZBoneList bones;
	ZAnimationMap animations;
	ZSkeleton skeleton;
	ZInstancedDataOptions instanceData;
	glm::mat4 globalInverseTransform;
	ZAABBox bounds;

	std::shared_ptr<ZRenderStateGroup> renderState;
	std::shared_ptr<ZUniformBuffer> uniformBuffer;

    ZModel();
    virtual ~ZModel() = default;

    virtual void OnCreate() { };
    virtual void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) { };

private:

	static ZIDSequence idGenerator_;
};

class ZModelManager : public ZResourceManager<ZModel, ZHModel>
{

public:

    ZModelManager() = default;
	~ZModelManager() = default;

	virtual void Initialize() override;
    virtual void CleanUp() override;

    ZHModel Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
    void DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode);
    ZHModel Create(const ZModelType& type, const ZHModel& restoreHandle = ZHModel(), const std::shared_ptr<ZOFObjectNode>& deserializedNode = nullptr);
    ZHModel Create(const std::string& path, const ZHModel& restoreHandle = ZHModel(), const std::shared_ptr<ZOFObjectNode>& deserializedNode = nullptr);
    void CreateAsync(const std::string& path, const ZHModel& restoreHandle = ZHModel(), const std::shared_ptr<ZOFObjectNode>& deserializedNode = nullptr);

    const std::string& Name(const ZHModel& handle);
    const std::string& Path(const ZHModel& handle);
    const ZMesh3DList& Meshes(const ZHModel& handle);
    const ZBoneList& Bones(const ZHModel& handle);
    const ZBoneMap& BonesMap(const ZHModel& handle);
    const ZAnimationMap& Animations(const ZHModel& handle);
    const ZInstancedDataOptions& InstanceData(const ZHModel& handle);
	const ZSkeleton Skeleton(const ZHModel& handle);
    const glm::mat4 GlobalInverseTransform(const ZHModel& handle);
    const ZAABBox& Bounds(const ZHModel& handle);
    const std::shared_ptr<ZRenderStateGroup> RenderState(const ZHModel& handle);

    void SetInstanceData(const ZHModel& handle, const ZInstancedDataOptions& instanceData);
    void BoneTransform(const ZHModel& handle, const std::string& anim, double secondsTime);

protected:

    void ComputeBounds(const ZHModel& handle);
    void CalculateTransformsInHierarchy(const ZHModel& handle, const std::string& animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform);
    glm::vec3 CalculateInterpolatedScaling(double animationTime, const ZJointAnimation& jointAnim);
    glm::quat CalculateInterpolatedRotation(double animationTime, const ZJointAnimation& jointAnim);
    glm::vec3 CalculateInterpolatedPosition(double animationTime, const ZJointAnimation& jointAnim);

    void HandleModelLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);

protected:

    std::unordered_map<unsigned int, std::shared_ptr<ZOFObjectNode>> asyncDeserializeDataNodes_;

};
