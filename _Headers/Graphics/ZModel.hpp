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
#include "ZCommon.hpp"
#include "ZEngine.hpp"
#include "ZMesh3D.hpp"
#include "ZAABBox.hpp"

// Forward Declarations
class ZShader;
struct ZSkeleton;
struct ZJoint;
struct ZJointAnimation;

// Class Definitions
class ZModel {
    
private:
    
    ZMesh3DMap meshes_;
    ZBoneMap bonesMap_;
    ZBoneList bones_;
    ZAnimationMap animations_;
    std::shared_ptr<ZSkeleton> skeleton_;
    ZAABBox boundingBox_;
    
    glm::mat4 globalInverseTransform_;
    
    void CreateGround(glm::vec3 scale);
    void CreateCube(glm::vec3 scale);
    void CreateSphere(glm::vec3 scale);
    void CreateCylinder(glm::vec3 scale);
    void CreateCone(glm::vec3 scale);
    
public:
    
    ZModel(ZPrimitiveType primitiveType, glm::vec3 scale = glm::vec3(1.0f, 0.f, 1.0f));
    ZModel() { }
    virtual ~ZModel() { }
    
    void Initialize(std::string path);
    
    virtual void Render(ZShader* shader);
    virtual void Render(ZShader* shader, std::vector<std::shared_ptr<ZMaterial>> materials);
    
    const ZMesh3DMap& Meshes() { return meshes_; }
    const ZAABBox& AABB() { return boundingBox_; }
    ZBoneList& Bones() { return bones_; }
    ZBoneMap& BonesMap() { return bonesMap_; }
    ZAnimationMap& Animations() { return animations_; }
    const std::shared_ptr<ZSkeleton> Skeleton() { return skeleton_; }
    const glm::mat4 GlobalInverseTransform() { return globalInverseTransform_; }
    
    virtual void InitializeAABB();
    virtual void UpdateAABB(glm::mat4 transform);
    
    void BoneTransform(std::string anim, double secondsTime);
    
    static std::unique_ptr<ZModel> NewGroundPrimitive(glm::vec3 scale = glm::vec3(1.0f, 0.f, 1.0f));
    static std::unique_ptr<ZModel> NewCubePrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewSpherePrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewCylinderPrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewConePrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
    static std::unique_ptr<ZModel> NewSkybox(ZIBLTexture& generatedIBLTexture, std::vector<std::string> faces = ZEngine::DEFAULT_SKYBOX_CUBEMAP);
    static std::unique_ptr<ZModel> NewSkybox(std::string equirectHDR, ZIBLTexture& generatedIBLTexture);
    
protected:
    
    void CalculateTransformsInHierarchy(std::string animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform);
    glm::vec3 CalculateInterpolatedScaling(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    glm::quat CalculateInterpolatedRotation(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    glm::vec3 CalculateInterpolatedPosition(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim);
    
};
