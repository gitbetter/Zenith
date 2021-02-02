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

// Includes
#include "ZMesh3D.hpp"
#include "ZTexture.hpp"
#include "ZAABBox.hpp"

// Forward Declarations
class ZShader;
struct ZSkeleton;
struct ZJoint;
struct ZJointAnimation;
class ZResourceLoadedEvent;

// Class Definitions
class ZModel : public std::enable_shared_from_this<ZModel>
{

    using Creator = std::unique_ptr<ZModel>(*)(const glm::vec3&);

private:

    std::string id_;
    std::string modelPath_;
    ZMesh3DMap meshes_;
    ZBoneMap bonesMap_;
    ZBoneList bones_;
    ZAnimationMap animations_;
    std::shared_ptr<ZSkeleton> skeleton_;
    ZAABBox boundingBox_;
    ZInstancedDataOptions instanceData_;

    glm::mat4 globalInverseTransform_;

    void CreatePlane(const glm::vec3& scale);
    void CreateCube(const glm::vec3& scale);
    void CreateSphere(const glm::vec3& scale);
    void CreateCylinder(const glm::vec3& scale);
    void CreateCone(const glm::vec3& scale);

public:

    ZModel(ZPrimitiveType primitiveType, const glm::vec3& scale = glm::vec3(1.0f, 0.f, 1.0f));
    ZModel(const std::string& path = "");
    virtual ~ZModel() {}

    void Initialize();
    void InitializeAsync();
    virtual void Render(const std::shared_ptr<ZShader>& shader);
    virtual void Render(const std::shared_ptr<ZShader>& shader, const std::vector<std::shared_ptr<ZMaterial>>& materials);
    virtual void InitializeAABB();
    virtual void UpdateAABB(const glm::mat4& transform);
    void SetInstanceData(const ZInstancedDataOptions& instanceData);
    void BoneTransform(const std::string& anim, double secondsTime);

    const std::string& ID() const { return id_; }
    const std::string& Path() const { return modelPath_; }
    const ZMesh3DMap& Meshes() const { return meshes_; }
    const ZAABBox& AABB() const { return boundingBox_; }
    const ZBoneList& Bones() const { return bones_; }
    const ZBoneMap& BonesMap() const { return bonesMap_; }
    const ZAnimationMap& Animations() const { return animations_; }
    const ZInstancedDataOptions& InstanceData() const { return instanceData_; }
    const std::shared_ptr<ZSkeleton> Skeleton() const { return skeleton_; }
    const glm::mat4 GlobalInverseTransform() const { return globalInverseTransform_; }

    static void CreateAsync(std::shared_ptr<ZOFTree> data, ZModelIDMap& outPendingModels);
    static void Create(std::shared_ptr<ZOFTree> data, ZModelMap& outModelMap);
    static std::unique_ptr<ZModel> Create(const std::string& type, const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));

    static std::unique_ptr<ZModel> NewPlanePrimitive(const glm::vec3& scale = glm::vec3(1.0f, 0.f, 1.0f));
    static std::unique_ptr<ZModel> NewCubePrimitive(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewSpherePrimitive(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewCylinderPrimitive(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewConePrimitive(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewSkybox(ZIBLTexture& generatedIBLTexture, const std::vector<std::string>& faces = DEFAULT_SKYBOX_CUBEMAP);
    static std::unique_ptr<ZModel> NewSkybox(const std::shared_ptr<ZTexture>& cubeMap, const std::shared_ptr<ZFramebuffer>& bufferData, ZIBLTexture& generatedIBLTexture);

protected:

    static std::map<std::string, Creator> modelCreators_;

    void CalculateTransformsInHierarchy(const std::string& animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform);
    glm::vec3 CalculateInterpolatedScaling(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    glm::quat CalculateInterpolatedRotation(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    glm::vec3 CalculateInterpolatedPosition(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);

    void HandleModelLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);

    static ZIDSequence idGenerator_;

};
