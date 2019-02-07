//
//  ZGLModelImporter.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGLModelImporter.hpp"
#include "ZLogger.hpp"
#include "ZMaterial.hpp"
#include <assimp/Importer.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "stb_image.hpp"

// Store already loaded textures in a map, since lookups are faster, and we may have many
std::unordered_map<std::string, ZTexture> ZGLModelImporter::loadedTextures;

/**
    Loads the model at the given path and outputs a set of meshes into the last parameter.

    @param shaderPath the path to the model file.
    @param outMeshes the mesh vector to populate.
*/
void ZGLModelImporter::LoadModel(std::string modelPath, std::vector<ZGLMesh3D>& outMeshes) {
  // Attempt to read the file
  // TODO: Might want to add more ReadFile Assimp flags such as aiProcess_GenNormals and aiProcess_OptimizeMeshes
  Assimp::Importer import;
  const aiScene* scene = import.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

  // The file might have incomplete data or no nodes to traverse. Handle that.
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    ZLogger::Log(std::string("ZGLModelImporter Error: ") + import.GetErrorString(), ZLoggerSeverity::Error);
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
void ZGLModelImporter::ProcessNode(aiNode* node, const aiScene* scene, std::string directory, std::vector<ZGLMesh3D>& outMeshes) {
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
    Zenith data structures to finally return a ZGLMesh3D.

    @param mesh the mesh to process.
    @param scene the aiScene that the mesh is a part of.
    @param directory the model directory, used for loading the textures for the materials.
    @return a ZGLMesh3D instance with all the relevant data
*/
ZGLMesh3D ZGLModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory) {
  std::vector<ZVertex> vertices;
  std::vector<unsigned int> indices;
  ZMaterial material;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    ZVertex vertex;

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

  if (mesh->mMaterialIndex >= 0) {
    std::vector<ZTexture> textures;
    aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<ZTexture> diffuseMaps = LoadMaterialTextures(meshMaterial, aiTextureType_DIFFUSE, "diffuse", directory);
    std::vector<ZTexture> specularMaps = LoadMaterialTextures(meshMaterial, aiTextureType_SPECULAR, "specular", directory);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    material = ZMaterial(textures);
  } else {
    material = ZMaterial::DefaultMaterial();
  }

  return ZGLMesh3D(vertices, indices, material);
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
    if (loadedTextures.find(textureName) != loadedTextures.end()) {
      textures.push_back(loadedTextures[textureName]);
    } else {
      ZTexture texture;
      texture.id = TextureFromFile(textureName, directory);
      texture.type = typeName;
      texture.path = textureName;
      textures.push_back(texture);
    }
  }
  return textures;
}

unsigned int ZGLModelImporter::TextureFromFile(std::string path, const std::string &directory) {
  std::string filename = directory + '/' + path;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
      GLenum format = 0;
      if (nrComponents == 1) format = GL_RED;
      else if (nrComponents == 3) format = GL_SRGB;
      else if (nrComponents == 4) format = GL_SRGB_ALPHA;

      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    ZLogger::Log("ZGLModelImporter Error: Failed to load texture at " + path, ZLoggerSeverity::Error);
  }

  stbi_image_free(data);

  return textureID;
}
