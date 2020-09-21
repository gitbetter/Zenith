/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZModelImporter.hpp

    Created by Adrian Sanchez on 27/01/2019.
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
#include "ZModel.hpp"
#include "ZMesh3D.hpp"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

// Forward Declarations
struct ZTexture;
struct ZSkeleton;
struct ZJoint;
struct ZBone;

// Class and Data Structure Definitions
class ZModelImporter
{

private:

    void ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory, ZMesh3DMap& outMeshes);
    std::shared_ptr<ZMesh3D> ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);

    std::vector<ZVertex3D> LoadVertexData(const aiMesh* mesh);
    std::vector<unsigned int> LoadIndexData(const aiMesh* mesh);
    void LoadSkeleton(const aiScene* scene);
    aiNode* FindSkeletonRoot(aiNode* rootNode);
    std::shared_ptr<ZJoint> LoadSkeletonJoint(const aiNode* node);
    void LoadBones(const aiMesh* mesh, std::vector<ZVertex3D>& vertices);
    void LoadAnimations(const aiScene* scene);
    std::vector<ZTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, const std::string& directory);

public:

    ZMesh3DMap LoadModel(const std::string& modelPath, ZBoneMap& outBoneMap, ZBoneList& outBoneList, ZAnimationMap& outAnimationMap, std::shared_ptr<ZSkeleton>& outSkeleton);
    ZMesh3DMap LoadModel(const std::shared_ptr<ZResourceHandle>& modelHandle, ZBoneMap& outBoneMap, ZBoneList& outBoneList, ZAnimationMap& outAnimationMap, std::shared_ptr<ZSkeleton>& outSkeleton, const std::string& modelDirectory = "");

    glm::vec3 AssimpToGLMVec3(const aiVector3D& vec);
    glm::quat AssimpToGLMQuat(const aiQuaternion& quat);
    glm::mat4 AssimpToGLMMat4(const aiMatrix4x4& mat);

protected:

    ZBoneMap currentBonesMap_;
    ZBoneList currentBones_;
    ZAnimationMap currentAnimations_;
    std::shared_ptr<ZSkeleton> currentSkeleton_;

};
