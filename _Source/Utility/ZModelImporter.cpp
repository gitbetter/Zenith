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
#include "ZResource.hpp"
#include "ZResourceCache.hpp"
#include "ZSkeleton.hpp"
#include "ZAnimation.hpp"
#include <assimp/Importer.hpp>

/**
 Loads the model at the given path and outputs a set of meshes into the last parameter.
 
 @param shaderPath the path to the model file.
 @param outMeshes the mesh vector to populate.
 */
void ZModelImporter::LoadModel(std::string modelPath, ZMesh3DMap& outMeshes, ZBoneMap& outBoneMap, ZBoneList& outBoneList) {
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
    outBoneMap = currentBonesMap_; outBoneList = currentBones_;
    
    // If we load another model with this ZModelImporter instance, we want to make sure there is no bone data
    // from previous loads.
    currentBonesMap_.clear(); currentBones_.clear();
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
	std::vector<ZVertex3D> vertices = LoadVertexData(mesh);
	std::vector<unsigned int> indices = LoadIndexData(mesh);
	// TODO: load bone vertex data by passing vertices to this call, instead of post-processing in the ZMesh3D instance
	LoadBones(mesh, vertices);
	std::shared_ptr<ZSkeleton> skeleton = LoadSkeleton(scene);
	ZAnimationMap animations = LoadAnimations(scene);
    
    std::shared_ptr<ZMesh3D> mesh3D = std::make_shared<ZMesh3D>(vertices, indices);
	mesh3D->SetSkeleton(skeleton);
	mesh3D->SetAnimations(animations);
    
    return mesh3D;
}

/**
 A helper function that loads all the vertex data for a specific aiMesh instance into a list
 of ZVertex3D objects and returns it.

 @param mesh the mesh from which the vertices are loaded.
 @return a vector of ZVertex3D objects, one for each vertex in the mesh.
 */
std::vector<ZVertex3D> ZModelImporter::LoadVertexData(const aiMesh* mesh) {
	std::vector<ZVertex3D> vertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		ZVertex3D vertex;

		vertex.position = ASSIMP_TO_GLM_VEC3(mesh->mVertices[i]);
		vertex.normal = ASSIMP_TO_GLM_VEC3(mesh->mNormals[i]);
		vertex.uv = glm::vec2(0.0f, 0.0f);

		if (mesh->mTextureCoords[0]) {
			glm::vec2 uv;
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = uv;
		}

		if (mesh->mTangents) {
			vertex.tangent = ASSIMP_TO_GLM_VEC3(mesh->mTangents[i]);
		}

		if (mesh->mBitangents) {
			vertex.bitangent = ASSIMP_TO_GLM_VEC3(mesh->mBitangents[i]);
		}

		vertices.push_back(vertex);
	}
	return vertices;
}

/**
 A helper function that loads all the vertex index data for the faces of a specific aiMesh instance.

 @param mesh the mesh from which the indices are loaded.
 @return a vector of unsigned integers, which specify the indexes of each vertex for every face of the mesh.
 */
std::vector<unsigned int> ZModelImporter::LoadIndexData(const aiMesh* mesh) {
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	return indices;
}

/**
 A helper function that creates a skeleton using a top level aiScene object.

 @param scene the scene from which to look for nodes to create the skeleton with.
 @return a skeleton that mirrors the hierarchy within the aiScene.
 */
std::shared_ptr<ZSkeleton> ZModelImporter::LoadSkeleton(const aiScene* scene) {
	std::shared_ptr<ZSkeleton> skeleton = std::make_shared<ZSkeleton>();
	skeleton->rootJoint = LoadSkeletonJoint(scene->mRootNode);
	return skeleton;
}

/**
 A recursive helper function that creates a joint and all child joints and links them accordingly.

 @param node the node from which to create a skeletal joint object.
 @return a ZJoint object.
 */
std::shared_ptr<ZJoint> ZModelImporter::LoadSkeletonJoint(const aiNode* node) {
	std::shared_ptr<ZJoint> joint = std::make_shared<ZJoint>();
	joint->name = std::string(node->mName.data);
	joint->transform = ASSIMP_TO_GLM_MAT4(node->mTransformation);
	
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		std::shared_ptr<ZJoint> childJoint = LoadSkeletonJoint(node->mChildren[i]);
		joint->children.push_back(childJoint);
		childJoint->parent = joint.get();
	}

	return joint;
}

/**
 A helper function that creates a map of ZBone objects if they exist for a mesh.

 @param mesh the mesh from which to look for bones.
 @return a map of bone objects, where the key is the bone name.
 */
void ZModelImporter::LoadBones(const aiMesh* mesh, std::vector<ZVertex3D>& vertices) {
	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		unsigned int boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.data);

		if (currentBonesMap_.find(boneName) == currentBonesMap_.end()) {
			boneIndex = currentBones_.size();
			std::shared_ptr<ZBone> bone = std::make_shared<ZBone>();
            currentBones_.push_back(bone);
		} else {
			boneIndex = currentBonesMap_[boneName];
		}

		currentBonesMap_[boneName] = boneIndex;
		currentBones_[boneIndex]->offset = ASSIMP_TO_GLM_MAT4(mesh->mBones[i]->mOffsetMatrix);
        currentBones_[boneIndex]->transformation = glm::mat4(1.f);

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
            vertices[vertexID].AddBoneData(boneIndex, weight);
		}
	}
}

/**
 A helper function that loads all of the animations for the mesh. The custom data structures reflect
 the Assimp animation data structures so the translation is quite basic.

 @param scene the scene from which to to look for animations.
 @return a list of ZAnimation STL pointers.
 */
ZAnimationMap ZModelImporter::LoadAnimations(const aiScene* scene) {
	ZAnimationMap animations;
	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* anim = scene->mAnimations[i];
		std::shared_ptr<ZAnimation> animation = std::make_shared<ZAnimation>();
		animation->name = std::string(anim->mName.data);
        if (animation->name.empty()) animation->name = "z_anim" + std::to_string(i + 1);
		animation->ticksPerSecond = anim->mTicksPerSecond;
		animation->duration = anim->mDuration;

		for (unsigned int j = 0; j < anim->mNumChannels; j++) {
			aiNodeAnim* nodeAnim = anim->mChannels[j];
			std::shared_ptr<ZJointAnimation> jointAnimation = std::make_shared<ZJointAnimation>();
			jointAnimation->jointName = std::string(nodeAnim->mNodeName.data);

			for (unsigned int k = 0; k < nodeAnim->mNumScalingKeys; k++) {
				ZAnimationKey<glm::vec3> scalingKey;
				scalingKey.time = nodeAnim->mScalingKeys[k].mTime;
				scalingKey.value = ASSIMP_TO_GLM_VEC3(nodeAnim->mScalingKeys[k].mValue);
				jointAnimation->scalingKeys.push_back(scalingKey);
			}

			for (unsigned int k = 0; k < nodeAnim->mNumRotationKeys; k++) {
				ZAnimationKey<glm::quat> rotationKey;
				rotationKey.time = nodeAnim->mRotationKeys[k].mTime;
				rotationKey.value = ASSIMP_TO_GLM_QUAT(nodeAnim->mRotationKeys[k].mValue);
				jointAnimation->rotationKeys.push_back(rotationKey);
			}

			for (unsigned int k = 0; k < nodeAnim->mNumPositionKeys; k++) {
				ZAnimationKey<glm::vec3> positionKey;
				positionKey.time = nodeAnim->mPositionKeys[k].mTime;
				positionKey.value = ASSIMP_TO_GLM_VEC3(nodeAnim->mPositionKeys[k].mValue);
				jointAnimation->positionKeys.push_back(positionKey);
			}

			animation->channels.push_back(jointAnimation);
		}

		animations[animation->name] = animation;
	}
	return animations;
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
