/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGLModelImporter.cpp

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

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZGLModelImporter.hpp"
#include "ZMaterial.hpp"
#include "ZResource.hpp"
#include "ZResourceCache.hpp"
#include <assimp/Importer.hpp>

/**
    Loads the model at the given path and outputs a set of meshes into the last parameter.

    @param shaderPath the path to the model file.
    @param outMeshes the mesh vector to populate.
*/
void ZGLModelImporter::LoadModel(std::string modelPath, ZMesh3DMap& outMeshes) {
  // Cache in the model data from the given file
  ZResource resource(modelPath);
  std::shared_ptr<ZResourceHandle> handle = ZEngine::ResourceCache()->GetHandle(&resource);

  // TODO: Might want to add more ReadFile Assimp flags such as aiProcess_GenNormals and aiProcess_OptimizeMeshes
  Assimp::Importer import;
  const aiScene* scene = import.ReadFileFromMemory((const void*)handle->Buffer(), (size_t)handle->Size(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  // The file might have incomplete data or no nodes to traverse. Handle that.
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    _Z(std::string("ZGLModelImporter Error: ") + import.GetErrorString(), ZERROR);
    return;
  }

  // Start processing nodes
  std::string modelDirectory = modelPath.substr(0, modelPath.find_last_of("/\\"));
  ProcessNode(scene->mRootNode, scene, modelDirectory, outMeshes);
}

/**
    A helper function that recursively processes an Assimp scene node and
    populates the provided final out parameter.

    @param shaderPath the path to the model file.
    @param outMeshes the mesh vector to populate.
*/
void ZGLModelImporter::ProcessNode(aiNode* node, const aiScene* scene, std::string directory, ZMesh3DMap& outMeshes) {
  // Process the node's meshes and add them to the out parameter
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    std::shared_ptr<ZMesh3D> outMesh = ProcessMesh(mesh, scene, directory);
    outMeshes[outMesh->ID()] = outMesh;
  }

  // Recursively visit the node's children in order to process their meshes
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[i], scene, directory, outMeshes);
  }
}

/**
    A helper function that processes a single node mesh. The idea is to fetch
    the position, normal, and texture coordinates for each vertex, as well
    as the vertex indices and the materials for the mesh and populate all corresponding
    Zenith data structures to finally return a ZMesh3D.

    @param mesh the mesh to process.
    @param scene the aiScene that the mesh is a part of.
    @param directory the model directory, used for loading the textures for the materials.
    @return a ZMesh3D instance with all the relevant data
*/
std::shared_ptr<ZMesh3D> ZGLModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory) {
  std::vector<ZVertex3D> vertices;
  std::vector<unsigned int> indices;
  std::shared_ptr<ZMaterial> material;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    ZVertex3D vertex;

    glm::vec3 position;
    position.x = mesh->mVertices[i].x;
    position.y = mesh->mVertices[i].y;
    position.z = mesh->mVertices[i].z;
    vertex.position = position;

    glm::vec3 normal;
    normal.x = mesh->mNormals[i].x;
    normal.y = mesh->mNormals[i].y;
    normal.z = mesh->mNormals[i].z;
    vertex.normal = normal;

    if (mesh->mTextureCoords[0]) {
      glm::vec2 uv;
      uv.x = mesh->mTextureCoords[0][i].x;
      uv.y = mesh->mTextureCoords[0][i].y;
      vertex.uv = uv;
    } else {
      vertex.uv = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  return std::make_shared<ZMesh3D>(vertices, indices);
}

/**
    A helper function that gets all the textures for a specific texture type (i.e. specular, diffuse, height, etc).

    @param mat the material with the textures to process.
    @param type the type of texture we're looking for.
    @param typeName the texture type as a string.
    @param directory the directory that supposedly contains the textures. We'll see.
    @return a vector of ZTextures with all the relevant data
*/
std::vector<ZTexture> ZGLModelImporter::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory) {
  std::vector<ZTexture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    std::string textureName = std::string(str.C_Str());
    if (ZEngine::Graphics()->Textures().find(textureName) != ZEngine::Graphics()->Textures().end()) {
      textures.push_back(ZEngine::Graphics()->Textures()[textureName]);
    } else {
      ZTexture texture = ZEngine::Graphics()->Strategy()->LoadTexture(textureName, directory);
      texture.type = typeName;
      texture.path = textureName;
      textures.push_back(texture);
    }
  }
  return textures;
}
