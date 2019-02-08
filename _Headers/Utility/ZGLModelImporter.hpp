//
//  ZGLModelImporter.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh3D.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

// Forward Declarations
struct ZTexture;

// Class and Data Structure Definitions
class ZGLModelImporter {
private:
  static std::unordered_map<std::string, ZTexture> loadedTextures;

  void ProcessNode(aiNode* node, const aiScene* scene, std::string directory, std::vector<ZMesh3D>& outMeshes);
  ZMesh3D ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory);
  std::vector<ZTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory);
  unsigned int TextureFromFile(std::string path, const std::string& directory);

public:
  void LoadModel(std::string modelPath, std::vector<ZMesh3D>& outMeshes);

protected:

};
