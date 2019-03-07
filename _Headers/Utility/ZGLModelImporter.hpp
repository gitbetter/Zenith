//
//  ZGLModelImporter.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

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

// Class and Data Structure Definitions
class ZGLModelImporter {
private:

  void ProcessNode(aiNode* node, const aiScene* scene, std::string directory, ZMesh3DMap& outMeshes);
  std::shared_ptr<ZMesh3D> ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory);
  std::vector<ZTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory);
  unsigned int TextureFromFile(std::string path, const std::string& directory);

public:

  void LoadModel(std::string modelPath, ZMesh3DMap& outMeshes);

protected:


};
