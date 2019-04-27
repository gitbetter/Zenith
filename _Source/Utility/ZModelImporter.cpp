/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZModelImporter.cpp
 
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
#include "ZModelImporter.hpp"
#include "ZMaterial.hpp"
#include "ZResource.hpp"
#include "ZResourceCache.hpp"
#include <assimp/Importer.hpp>

/**
 Loads the model at the given path and outputs a set of meshes into the last parameter.
 
 @param shaderPath the path to the model file.
 @param outMeshes the mesh vector to populate.
 */
void ZModelImporter::LoadModel(std::string modelPath, ZMesh3DMap& outMeshes) {
    // Cache in the model data from the given file
    ZResource resource(modelPath);
    std::shared_ptr<ZResourceHandle> handle = ZEngine::ResourceCache()->GetHandle(&resource);
    
    // TODO: Might want to add more ReadFile Assimp flags such as aiProcess_GenNormals and aiProcess_OptimizeMeshes
    Assimp::Importer import;
    const aiScene* scene = import.ReadFileFromMemory((const void*)handle->Buffer(), (size_t)handle->Size(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );
    
    // The file might have incomplete data or no nodes to traverse. Handle that.
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        _Z(std::string("ZModelImporter Error: ") + import.GetErrorString(), ZERROR);
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
void ZModelImporter::ProcessNode(aiNode* node, const aiScene* scene, std::string directory, ZMesh3DMap& outMeshes) {
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
std::shared_ptr<ZMesh3D> ZModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory) {
    std::vector<ZVertex3D> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<ZMaterial> material;
    
    // Load basic vertex data
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
        
        if (mesh->mTangents) {
            glm::vec3 tangent;
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
            vertex.tangent = tangent;
        }
        
        if (mesh->mBitangents) {
            glm::vec3 bitangent;
            bitangent.x = mesh->mBitangents[i].x;
            bitangent.y = mesh->mBitangents[i].y;
            bitangent.z = mesh->mBitangents[i].z;
            vertex.bitangent = bitangent;
        }
        
        vertices.push_back(vertex);
    }
    
    // Load mesh indices for each face
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    std::shared_ptr<ZMesh3D> mesh3D = std::make_shared<ZMesh3D>(vertices, indices);
    mesh3D->SetAssimpScene(scene);
    
    // Load mesh bone data
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        unsigned int boneIndex = 0;
        std::string boneName(mesh->mBones[i]->mName.data);
        
        if (mesh3D->bonesMap_.find(boneName) == mesh3D->bonesMap_.end()) {
            boneIndex = mesh3D->bonesMap_.size();
            ZBoneInfo boneInfo;
            mesh3D->boneInfo_.push_back(boneInfo);
        } else {
            boneIndex = mesh3D->bonesMap_[boneName];
        }
        
        mesh3D->bonesMap_[boneName] = boneIndex;
        aiMatrix4x4 offset = mesh->mBones[i]->mOffsetMatrix;
        mesh3D->boneInfo_[boneIndex].boneOffset = ASSIMP_TO_GLM_MAT4(offset);
        
        for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            // TODO: vertexID might be duplicated if processing several meshes, so make sure
            // to make it unique somehow
            unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            mesh3D->vertices_[vertexID].boneData.AddBoneData(boneIndex, weight);
        }
    }
    
    return mesh3D;
}

/**
 A helper function that gets all the textures for a specific texture type (i.e. specular, diffuse, height, etc).
 
 @param mat the material with the textures to process.
 @param type the type of texture we're looking for.
 @param typeName the texture type as a string.
 @param directory the directory that supposedly contains the textures. We'll see.
 @return a vector of ZTextures with all the relevant data
 */
std::vector<ZTexture> ZModelImporter::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, std::string directory) {
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
