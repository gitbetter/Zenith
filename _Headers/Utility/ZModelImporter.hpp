//
//  ZModelImporter.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>
#include <string>
#include <unordered_map>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

// Forward Declarations
class ZMesh;
struct ZTexture;

// Class and Data Structure Definitions
class ZModelImporter {
private:
  static std::unordered_map<std::string, ZTexture> loadedTextures;

  void ProcessNode(aiNode* node, const aiScene* scene, std::string directory, std::vector<ZMesh>& outMeshes);
  ZMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory);
  std::vector<ZTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory);
  unsigned int TextureFromFile(std::string path, const std::string& directory);

public:
  void LoadModel(std::string modelPath, std::vector<ZMesh>& outMeshes);

protected:

};
