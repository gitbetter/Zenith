/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZModel.cpp
 
    Created by Adrian Sanchez on 1/25/19.
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
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZMesh3D.hpp"
#include "ZModelImporter.hpp"
#include "ZCommon.hpp"
#include "ZAnimation.hpp"
#include "ZSkeleton.hpp"
#include "ZResourceCache.hpp"
#include "ZEventAgent.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZModelReadyEvent.hpp"

ZModel::ZModel(ZPrimitiveType primitiveType, glm::vec3 scale) : globalInverseTransform_(glm::mat4(1.f)) {
    switch (primitiveType) {
        case ZPrimitiveType::Plane:
            CreateGround(scale); break;
        case ZPrimitiveType::Cube:
            CreateCube(scale); break;
        case ZPrimitiveType::Sphere:
            CreateSphere(scale); break;
        case ZPrimitiveType::Cylinder:
            CreateCylinder(scale); break;
        case ZPrimitiveType::Cone:
            CreateCone(scale); break;
    }
    Initialize();
}

void ZModel::Initialize() {
    if (!modelPath_.empty()) {
        ZModelImporter importer;
        meshes_ = importer.LoadModel(modelPath_, bonesMap_, bones_, animations_, skeleton_);
        for (ZMesh3DMap::iterator it = meshes_.begin(); it != meshes_.end(); it++) it->second->Initialize();
        if (skeleton_) globalInverseTransform_ = glm::inverse(skeleton_->rootJoint->transform);
    }
    InitializeAABB();
}

void ZModel::InitializeAsync() {
	ZResource modelResource(modelPath_, ZResourceType::Model);
	zenith::ResourceCache()->RequestHandle(modelResource);

	ZEventDelegate modelLoadDelegate = fastdelegate::MakeDelegate(this, &ZModel::HandleModelLoaded);
	zenith::EventAgent()->AddListener(modelLoadDelegate, ZResourceLoadedEvent::Type);
}

void ZModel::Render(ZShader* shader) {
    std::vector<std::shared_ptr<ZMaterial>> materials = { ZMaterial::DefaultMaterialPBR() };
    Render(shader, materials);
}

void ZModel::Render(ZShader* shader, std::vector<std::shared_ptr<ZMaterial>> materials) {
    shader->Activate();
    shader->SetBool("rigged", skeleton_ != nullptr);
    if (skeleton_) {
        shader->Use(bones_);
    }
    
    ZMesh3DMap meshesLeft = meshes_;
    for (auto materialIt = materials.begin(); materialIt != materials.end(); materialIt++) {
        if (!(*materialIt)->MeshID().empty()) {
            meshes_[(*materialIt)->MeshID()]->Render(shader, (*materialIt).get());
            meshesLeft.erase((*materialIt)->MeshID());
        } else {
            for (ZMesh3DMap::iterator meshIt = meshesLeft.begin(); meshIt != meshesLeft.end(); meshIt++) {
                meshIt->second->Render(shader, (*materialIt).get());
            }
        }
    }
}

void ZModel::InitializeAABB() {
    glm::vec3 min(0.f), max(0.f);
    
    ZMesh3DMap::iterator it = meshes_.begin(), end = meshes_.end();
    for (; it != end; it++) {
        const std::vector<ZVertex3D>& vertices = it->second->vertices_;
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices[i].position.x < min.x) min.x = vertices[i].position.x;
            if (vertices[i].position.x < min.y) min.y = vertices[i].position.y;
            if (vertices[i].position.x < min.z) min.z = vertices[i].position.z;
            if (vertices[i].position.x > max.x) max.x = vertices[i].position.x;
            if (vertices[i].position.x > max.y) max.y = vertices[i].position.y;
            if (vertices[i].position.x > max.z) max.z = vertices[i].position.z;
        }
    }
    
    boundingBox_.minimum = min;
    boundingBox_.maximum = max;
}

void ZModel::UpdateAABB(glm::mat4 transform) {
    boundingBox_.minimum = transform * glm::vec4(boundingBox_.minimum, 1.f);
    boundingBox_.maximum = transform * glm::vec4(boundingBox_.maximum, 1.f);
}

/**
 *  Plane Creation
 */
std::unique_ptr<ZModel> ZModel::NewGroundPrimitive(glm::vec3 scale) {
    return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Plane, scale));
}

void ZModel::CreateGround(glm::vec3 scale) {
	// TODO: Refactor the texture tiling. It does not belong in the model creation code.
	float textureTiling = 15.f;

    ZVertex3D topLeft(glm::vec3(-scale.x, 0.f, -scale.z)); topLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D bottomLeft(glm::vec3(-scale.x, 0.f, scale.z)); bottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D bottomRight(glm::vec3(scale.x, 0.f, scale.z)); bottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D topRight(glm::vec3(scale.x, 0.f, -scale.z)); topRight.uv = glm::vec2(textureTiling, textureTiling);
	ZGraphics::ComputeTangentBitangent(bottomLeft, bottomRight, topRight);
	ZGraphics::ComputeTangentBitangent(bottomLeft, topRight, topLeft);
    
    std::vector<ZVertex3D> vertices = {
        topLeft, bottomLeft, bottomRight, topRight
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };
    
    std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(vertices, indices);
    mesh->Initialize();
    meshes_[mesh->ID()] = mesh;
}

/**
 *  Cube Creation
 */
std::unique_ptr<ZModel> ZModel::NewCubePrimitive(glm::vec3 scale) {
    return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Cube, scale));
}

void ZModel::CreateCube(glm::vec3 scale) {
    // TODO: Refactor the texture tiling. It does not belong in the model creation code.
    float textureTiling = 15.f;
    
	// Front face
	ZVertex3D front_BottomLeft(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f)); front_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D front_BottomRight(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f)); front_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D front_TopRight(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f)); front_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D front_TopLeft(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f)); front_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZGraphics::ComputeTangentBitangent(front_BottomLeft, front_BottomRight, front_TopRight);
	ZGraphics::ComputeTangentBitangent(front_BottomLeft, front_TopRight, front_TopLeft);

	// Back face
	ZVertex3D back_BottomLeft(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f)); back_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D back_BottomRight(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f)); back_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D back_TopRight(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f)); back_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D back_TopLeft(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f)); back_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZGraphics::ComputeTangentBitangent(back_BottomLeft, back_BottomRight, back_TopRight);
	ZGraphics::ComputeTangentBitangent(back_BottomLeft, back_TopRight, back_TopLeft);

	// Right Face
	ZVertex3D right_BottomLeft(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(1.f, 0.f, 0.f)); right_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D right_BottomRight(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(1.f, 0.f, 0.f)); right_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D right_TopRight(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(1.f, 0.f, 0.f)); right_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D right_TopLeft(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(1.f, 0.f, 0.f)); right_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZGraphics::ComputeTangentBitangent(right_BottomLeft, right_BottomRight, right_TopRight);
	ZGraphics::ComputeTangentBitangent(right_BottomLeft, right_TopRight, right_TopLeft);

	// Left face
	ZVertex3D left_BottomLeft(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(-1.f, 0.f, 0.f)); left_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D left_BottomRight(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(-1.f, 0.f, 0.f)); left_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D left_TopRight(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(-1.f, 0.f, 0.f)); left_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D left_TopLeft(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(-1.f, 0.f, 0.f)); left_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZGraphics::ComputeTangentBitangent(left_BottomLeft, left_BottomRight, left_TopRight);
	ZGraphics::ComputeTangentBitangent(left_BottomLeft, left_TopRight, left_TopLeft);

	// Top face
	ZVertex3D top_BottomLeft(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(0.f, 1.f, 0.f)); top_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D top_BottomRight(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(0.f, 1.f, 0.f)); top_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D top_TopRight(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(0.f, 1.f, 0.f)); top_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D top_TopLeft(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(0.f, 1.f, 0.f)); top_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZGraphics::ComputeTangentBitangent(top_BottomLeft, top_BottomRight, top_TopRight);
	ZGraphics::ComputeTangentBitangent(top_BottomLeft, top_TopRight, top_TopLeft);

	// Bottom face
	ZVertex3D bottom_BottomLeft(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(0.f, -1.f, 0.f)); bottom_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D bottom_BottomRight(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(0.f, -1.f, 0.f)); bottom_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D bottom_TopRight(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(0.f, -1.f, 0.f)); bottom_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D bottom_TopLeft(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(0.f, -1.f, 0.f)); bottom_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZGraphics::ComputeTangentBitangent(bottom_BottomLeft, bottom_BottomRight, bottom_TopRight);
	ZGraphics::ComputeTangentBitangent(bottom_BottomLeft, bottom_TopRight, bottom_TopLeft);
    
    std::vector<ZVertex3D> vertices = {
        front_BottomLeft, front_BottomRight, front_TopRight, front_TopLeft,
        right_BottomLeft, right_BottomRight, right_TopRight, right_TopLeft,
        back_BottomLeft, back_BottomRight, back_TopRight, back_TopLeft,
        left_BottomLeft, left_BottomRight, left_TopRight, left_TopLeft,
        top_BottomLeft, top_BottomRight, top_TopRight, top_TopLeft,
        bottom_BottomLeft, bottom_BottomRight, bottom_TopRight, bottom_TopLeft
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };
    
    std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(vertices, indices);
    mesh->Initialize();
    meshes_[mesh->ID()] = mesh;
}

/**
 *  Sphere Creation
 */
std::unique_ptr<ZModel> ZModel::NewSpherePrimitive(glm::vec3 scale) {
    return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Sphere, scale));
}

void ZModel::CreateSphere(glm::vec3 scale) {
    std::vector<ZVertex3D> vertices;
    std::vector<unsigned int> indices;
    
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = glm::cos(xSegment * 2.0f * glm::pi<float>()) * glm::sin(ySegment * glm::pi<float>());
            float yPos = glm::cos(ySegment * glm::pi<float>());
            float zPos = glm::sin(xSegment * 2.0f * glm::pi<float>()) * glm::sin(ySegment * glm::pi<float>());
            
            ZVertex3D vertex(glm::vec3(xPos * scale.x, yPos * scale.y, zPos * scale.z), glm::vec3(xPos, yPos, zPos));
            vertex.uv = glm::vec2(xSegment, ySegment);
            
            vertices.push_back(vertex);
        }
    }
    
    bool oddRow = false;
    for (int y = 0; y < Y_SEGMENTS; ++y) {
        if (!oddRow) {
            for (int x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        } else {
            for (int x = X_SEGMENTS; x >= 0; --x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    
    std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(vertices, indices, ZMeshDrawStyle::TriangleStrip);
    mesh->Initialize();
    meshes_[mesh->ID()] = mesh;
}

/**
 *  Cylinder Creation
 */
std::unique_ptr<ZModel> ZModel::NewCylinderPrimitive(glm::vec3 scale) {
    return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Cylinder, scale));
}

void ZModel::CreateCylinder(glm::vec3 scale) {
    
}

/**
 *  Cone Creation
 */
std::unique_ptr<ZModel> ZModel::NewConePrimitive(glm::vec3 scale) {
    return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Cone, scale));
}

void ZModel::CreateCone(glm::vec3 scale) {
    
}

/**
 *  Skybox Creation
 */
std::unique_ptr<ZModel> ZModel::NewSkybox(ZIBLTexture& generatedIBLTexture, std::vector<std::string> faces) {
    generatedIBLTexture.cubeMap = zenith::Graphics()->Strategy()->LoadCubeMap(faces);
 
    return NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f));
}

std::unique_ptr<ZModel> ZModel::NewSkybox(ZTexture& cubeMap, ZBufferData& bufferData, ZIBLTexture& generatedIBLTexture) {
    generatedIBLTexture.cubeMap = cubeMap;
    generatedIBLTexture.irradiance = zenith::Graphics()->Strategy()->IrradianceMapFromCubeMap(bufferData, cubeMap);
    generatedIBLTexture.prefiltered = zenith::Graphics()->Strategy()->PrefilterCubeMap(bufferData, cubeMap);
    generatedIBLTexture.brdfLUT = zenith::Graphics()->Strategy()->BRDFLUT(bufferData);
    
    return NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f));
}

void ZModel::BoneTransform(std::string anim, double secondsTime) {
    glm::mat4 identity(1.f);
    
    if (animations_.find(anim) != animations_.end()) {
        double ticksPerSecond = animations_[anim]->ticksPerSecond != 0 ? animations_[anim]->ticksPerSecond : 25.0;
        double timeInTicks = secondsTime * ticksPerSecond;
        double animationTime = fmod(timeInTicks, animations_[anim]->duration);
        
        CalculateTransformsInHierarchy(anim, animationTime, skeleton_->rootJoint, identity);
    }
}

void ZModel::CalculateTransformsInHierarchy(std::string animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform) {
    std::shared_ptr<ZAnimation> animation = animations_[animName];
    glm::mat4 jointTransform = joint->transform;

	if (joint->name.find("root") != std::string::npos) {
		jointTransform = glm::rotate(jointTransform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
		jointTransform = glm::translate(jointTransform, glm::vec3(0.f, -0.337f, -0.175f));
	}

    std::shared_ptr<ZJointAnimation> jointAnimation;
    for (unsigned int i = 0, j = animation->channels.size(); i < j; i++) {
        std::shared_ptr<ZJointAnimation> anim = animation->channels[i];
        if (anim->jointName == joint->name) {
            jointAnimation = anim; break;
        }
    }
    
    if (jointAnimation) {
        glm::vec3 scale = CalculateInterpolatedScaling(animTime, jointAnimation);
        glm::quat rotation = CalculateInterpolatedRotation(animTime, jointAnimation);
        glm::vec3 position = CalculateInterpolatedPosition(animTime, jointAnimation);
        
        glm::mat4 rotationM(1.f), scalingM(1.f), translationM(1.f);
        
        rotationM = glm::mat4_cast(rotation);
        scalingM = glm::scale(scalingM, scale);
        translationM = glm::translate(translationM, position);
        
        jointTransform = translationM * rotationM * scalingM;
	}
    
    glm::mat4 globalTransform = parentTransform * jointTransform;
    
    if (bonesMap_.find(joint->name) != bonesMap_.end()) {
        unsigned int index = bonesMap_[joint->name];
        bones_[index]->transformation = globalInverseTransform_ * globalTransform * bones_[index]->offset;
    }
    
    for (unsigned int i = 0; i < joint->children.size(); i++) {
        CalculateTransformsInHierarchy(animName, animTime, joint->children[i], globalTransform);
    }
}

glm::vec3 ZModel::CalculateInterpolatedScaling(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim) {
    if (jointAnim->scalingKeys.size() == 1) {
        return jointAnim->scalingKeys[0].value;
    }
    
    unsigned int scalingIndex = 0;
    for (unsigned int i = 0, j = jointAnim->scalingKeys.size() - 1; i < j; i++) {
        if (animationTime < jointAnim->scalingKeys[i + 1].time) {
            scalingIndex = i; break;
        }
    }
    unsigned int nextScalingIndex = scalingIndex + 1;
    double deltaTime = jointAnim->scalingKeys[nextScalingIndex].time - jointAnim->scalingKeys[scalingIndex].time;
    double factor = (animationTime - jointAnim->scalingKeys[scalingIndex].time) / deltaTime;
    glm::vec3 start = jointAnim->scalingKeys[scalingIndex].value;
    glm::vec3 end = jointAnim->scalingKeys[nextScalingIndex].value;
    glm::vec3 delta = end - start;
    
    return start + (float)factor * delta;
}

glm::quat ZModel::CalculateInterpolatedRotation(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim) {
    if (jointAnim->rotationKeys.size() == 1) {
        return jointAnim->rotationKeys[0].value;
    }
    
    unsigned int rotationIndex = 0;
    for (unsigned int i = 0, j = jointAnim->rotationKeys.size() - 1; i < j; i++) {
        if (animationTime < jointAnim->rotationKeys[i + 1].time) {
            rotationIndex = i; break;
        }
    }
    unsigned int nextRotationIndex = rotationIndex + 1;
    double deltaTime = jointAnim->rotationKeys[nextRotationIndex].time - jointAnim->rotationKeys[rotationIndex].time;
    double factor = (animationTime - jointAnim->rotationKeys[rotationIndex].time) / deltaTime;
    glm::quat start = jointAnim->rotationKeys[rotationIndex].value;
    glm::quat end = jointAnim->rotationKeys[nextRotationIndex].value;
    glm::quat out = glm::mix(start, end, (float)factor);
    
    return glm::normalize(out);
}

glm::vec3 ZModel::CalculateInterpolatedPosition(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim) {
    if (jointAnim->positionKeys.size() == 1) {
        return jointAnim->positionKeys[0].value;
    }
    
    unsigned int positionIndex = 0;
    for (unsigned int i = 0, j = jointAnim->positionKeys.size() - 1; i < j; i++) {
        if (animationTime < jointAnim->positionKeys[i + 1].time) {
            positionIndex = i; break;
        }
    }
    unsigned int nextPositionIndex = positionIndex + 1;
    double deltaTime = jointAnim->positionKeys[nextPositionIndex].time - jointAnim->positionKeys[positionIndex].time;
    double factor = (animationTime - jointAnim->positionKeys[positionIndex].time) / deltaTime;
    glm::vec3 start = jointAnim->positionKeys[positionIndex].value;
    glm::vec3 end = jointAnim->positionKeys[nextPositionIndex].value;
    glm::vec3 delta = end - start;
    
    return start + (float)factor * delta;
}

void ZModel::HandleModelLoaded(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZResourceLoadedEvent> loaded = std::dynamic_pointer_cast<ZResourceLoadedEvent>(event);
	if (!loaded->Handle()) return;

	std::shared_ptr<ZModelResourceExtraData> extraData = std::dynamic_pointer_cast<ZModelResourceExtraData>(loaded->Handle()->ExtraData());

	if (loaded->Handle()->Resource().name == modelPath_) {
		meshes_ = extraData->Meshes();
		bonesMap_ = extraData->BoneMap();
		bones_ = extraData->Bones();
		animations_ = extraData->Animations();
		skeleton_ = extraData->Skeleton();
        
        for (ZMesh3DMap::iterator it = meshes_.begin(); it != meshes_.end(); it++) it->second->Initialize();
        if (skeleton_) globalInverseTransform_ = glm::inverse(skeleton_->rootJoint->transform);
        InitializeAABB();

		ZEventDelegate modelLoadDelegate = fastdelegate::MakeDelegate(this, &ZModel::HandleModelLoaded);
		zenith::EventAgent()->RemoveListener(modelLoadDelegate, ZResourceLoadedEvent::Type);

		std::shared_ptr<ZModelReadyEvent> modelReadyEvent = std::make_shared<ZModelReadyEvent>(shared_from_this());
		zenith::EventAgent()->QueueEvent(modelReadyEvent);
	}
}
