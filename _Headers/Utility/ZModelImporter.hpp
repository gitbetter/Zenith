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
#include "ZCommon.hpp"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

// Forward Declarations
struct ZTexture;
struct ZSkeleton;
struct ZJoint;
struct ZBone;

// Class and Data Structure Definitions
class ZModelImporter {
private:

  void ProcessNode(aiNode* node, const aiScene* scene, std::string directory, ZMesh3DMap& outMeshes);
  std::shared_ptr<ZMesh3D> ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory);

  std::vector<ZVertex3D> LoadVertexData(const aiMesh* mesh);
  std::vector<unsigned int> LoadIndexData(const aiMesh* mesh);
  std::shared_ptr<ZSkeleton> LoadSkeleton(const aiScene* scene);
  std::shared_ptr<ZJoint> LoadSkeletonJoint(const aiNode* node);
  ZBoneMap LoadBones(const aiMesh* mesh);
  ZAnimationMap LoadAnimations(const aiScene* scene);
  std::vector<ZTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory);

public:

  void LoadModel(std::string modelPath, ZMesh3DMap& outMeshes);

protected:


};
