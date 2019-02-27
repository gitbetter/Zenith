//
//  ZGLModelImporter.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZGLModelImporter.hpp"
#include "ZCommon.hpp"
#include "ZMaterial.hpp"
#include <glm/vec3.hpp>
#include <assimp/Importer.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Store already loaded textures in a map, since lookups are faster, and we may have many
std::unordered_map<std::string, ZTexture> ZGLModelImporter::loadedTextures;

/**
    Loads the model at the given path and outputs a set of meshes into the last parameter.

    @param shaderPath the path to the model file.
    @param outMeshes the mesh vector to populate.
*/
void ZGLModelImporter::LoadModel(std::string modelPath, std::vector<ZMesh3D>& outMeshes) {
  // Attempt to read the file
  // TODO: Might want to add more ReadFile Assimp flags such as aiProcess_GenNormals and aiProcess_OptimizeMeshes
  Assimp::Importer import;
  const aiScene* scene = import.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

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
void ZGLModelImporter::ProcessNode(aiNode* node, const aiScene* scene, std::string directory, std::vector<ZMesh3D>& outMeshes) {
  // Process the node's meshes and add them to the out parameter
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    outMeshes.push_back(ProcessMesh(mesh, scene, directory));
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
ZMesh3D ZGLModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory) {
  std::vector<ZVertex3D> vertices;
  std::vector<unsigned int> indices;
  ZMaterial material;

  ZVertex3D minVertex((glm::vec3(std::numeric_limits<float>::max())));
  ZVertex3D maxVertex((glm::vec3(std::numeric_limits<float>::min())));

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

    if (vertex.position.x < minVertex.position.x) minVertex.position.x = vertex.position.x;
    if (vertex.position.y < minVertex.position.y) minVertex.position.y = vertex.position.y;
    if (vertex.position.z < minVertex.position.z) minVertex.position.z = vertex.position.z;

    if (vertex.position.x > maxVertex.position.x) maxVertex.position.x = vertex.position.x;
    if (vertex.position.y > maxVertex.position.y) maxVertex.position.y = vertex.position.y;
    if (vertex.position.z > maxVertex.position.z) maxVertex.position.z = vertex.position.z;

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    std::vector<ZTexture> textures;
    aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<ZTexture> diffuseMaps = LoadMaterialTextures(meshMaterial, aiTextureType_DIFFUSE, "diffuse", directory);
    std::vector<ZTexture> specularMaps = LoadMaterialTextures(meshMaterial, aiTextureType_SPECULAR, "specular", directory);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    material = ZMaterial(textures);
  } else {
    material = ZMaterial::DefaultMaterialSimple();
  }

  return ZMesh3D(vertices, indices, material, minVertex, maxVertex);
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
