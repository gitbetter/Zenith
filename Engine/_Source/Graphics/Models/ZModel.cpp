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

#include "ZModel.hpp"
#include "ZFramebuffer.hpp"
#include "ZPlane.hpp"
#include "ZCube.hpp"
#include "ZSphere.hpp"
#include "ZCylinder.hpp"
#include "ZMaterial.hpp"
#include "ZModelImporter.hpp"
#include "ZAnimation.hpp"
#include "ZSkeleton.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"
#include "ZServices.hpp"

ZIDSequence ZModel::idGenerator_;

ZModel::ZModel()
{
    name = "Model_" + std::to_string(idGenerator_.Next());
}

ZHModel ZModelManager::Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
    if (dataNode->type != ZOFObjectType::Model)
    {
        return ZHModel();
    }

	if (dataNode->properties.find("type") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> typeProp = dataNode->properties["type"]->Value<ZOFString>(0);
		std::shared_ptr<ZModel> model = ZModel::Create(typeProp->value, it->second);
		model->id_ = it->first;
		models[model->id_] = model;
	}
	else if (dataNode->properties.find("path") != dataNode->properties.end()) {
		std::shared_ptr<ZOFString> pathProp = dataNode->properties["path"]->Value<ZOFString>(0);
		std::shared_ptr<ZModel> model = ZModel::CreateExternal(pathProp->value);
		model->id_ = it->first;
		models[model->id_] = model;
	}
}

void ZModelManager::DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
    for (ZOFChildList::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        std::shared_ptr<ZOFObjectNode> dataNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
        if (dataNode->type == ZOFObjectType::Model)
        {
            std::string path;
            if (dataNode->properties.find("type") != dataNode->properties.end())
            {
                std::shared_ptr<ZOFString> typeProp = dataNode->properties["type"]->Value<ZOFString>(0);
                std::shared_ptr<ZModel> model = ZModel::Create(typeProp->value, it->second);
                model->id_ = it->first;
                outModelMap[model->id_] = model;
            }
            if (dataNode->properties.find("path") != dataNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = dataNode->properties["path"]->Value<ZOFString>(0);
                std::shared_ptr<ZModel> model = ZModel::CreateExternal(pathStr->value, true);
                model->id_ = it->first;
                outPendingModels[model] = it->first;
            }
        }
    }
}

std::shared_ptr<ZModel> ZModelManager::Create(const std::string& type, const std::shared_ptr<ZOFNode>& data)
{
    std::shared_ptr<ZModel> model = nullptr;
    if (type == "Plane")
    {
        model = std::make_shared<ZPlane>();
    }
    else if (type == "Cube") {
        model = std::make_shared<ZCube>();
    }
    else if (type == "Sphere") {
        model = std::make_shared<ZSphere>();
    }
    else if (type == "Cylinder") {
        model = std::make_shared<ZCylinder>();
    }
    else if (type == "Cone") {
        //model = ZCone::Create();
    }

    if (model) {
        if (data)
            model->Initialize(data);
        else
            model->Initialize();
    }
    else {
        LOG("Model of type " + type + " is not supported.", ZSeverity::Warning);
    }
    return model;
}

std::shared_ptr<ZModel> ZModelManager::CreateExternal(const std::string& path, bool async)
{
    std::shared_ptr<ZModel> model = std::make_shared<ZModel>(path);
    if (async)
        model->InitializeAsync();
    else
        model->Initialize();
    return model;
}

void ZModelManager::Initialize()
{
    if (!modelPath_.empty() && meshes_.empty())
    {
        ZModelImporter importer;
        meshes_ = importer.LoadModel(modelPath_, bonesMap_, bones_, animations_, skeleton_);
        for (ZMesh3DMap::iterator it = meshes_.begin(); it != meshes_.end(); it++) it->second->Initialize();
        if (skeleton_ && skeleton_->rootJoint) globalInverseTransform_ = glm::inverse(skeleton_->rootJoint->transform);
    }

    ComputeBounds();

    bool isRigged = !bonesMap_.empty();
    uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::Model, sizeof(ZModelUniforms));
    uniformBuffer_->Update(offsetof(ZModelUniforms, rigged), sizeof(isRigged), &isRigged);

    for (auto i = 0; i < bones_.size(); i++) {
        uniformBuffer_->Update(offsetof(ZModelUniforms, bones) + sizeof(glm::mat4) * i, sizeof(glm::mat4), glm::value_ptr(bones_[i]->transformation));
    }

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();

    std::shared_ptr<ZModelReadyEvent> modelReadyEvent = std::make_shared<ZModelReadyEvent>(shared_from_this());
    ZServices::EventAgent()->Queue(modelReadyEvent);
}

void ZModelManager::Initialize(const std::shared_ptr<ZOFNode>& data)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(data);
    if (!node)
    {
        LOG("Could not initalize ZPlane: node data is invalid", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("path") != props.end() && props["path"]->HasValues())
    {
        std::shared_ptr<ZOFString> pathProp = props["path"]->Value<ZOFString>(0);
        modelPath_ = pathProp->value;
    }

    Initialize();
}

void ZModelManager::InitializeAsync()
{
    ZResourceData::ptr modelResource = std::make_shared<ZResourceData>(modelPath_, ZResourceType::Model);
    ZServices::ResourceImporter()->GetDataAsync(modelResource);

    ZServices::EventAgent()->Subscribe(this, &ZModel::HandleModelLoaded);
}

bool ZModelManager::IsLoaded(const std::string& name)
{
    return loadedModels_.find(name) != loadedModels_.end();
}

ZHModel ZModelManager::GetFromName(const std::string& name)
{
	if (loadedModels_.find(name) != loadedModels_.end()) {
		return loadedModels_[name];
	}
	return ZHModel();
}

void ZModelManager::SetInstanceData(const ZInstancedDataOptions& instanceData)
{
    instanceData_ = instanceData;
    bool isInstanced = instanceData_.count > 1;
    uniformBuffer_->Update(offsetof(ZModelUniforms, instanced), sizeof(isInstanced), &isInstanced);
    for (auto it = meshes_.begin(); it != meshes_.end(); it++)
    {
        it->second->SetInstanceData(instanceData);
    }
}

void ZModelManager::ComputeBounds()
{
    bounds_ = ZAABBox();

    auto it = meshes_.cbegin(), end = meshes_.cend();
    for (; it != end; it++)
    {
        const ZVertex3DList& vertices = it->second->Vertices();
        for (int i = 0; i < vertices.size(); i++)
        {
            bounds_ = ZAABBox::Union(bounds_, vertices[i].position);
        }
    }
}

void ZModelManager::BoneTransform(const std::string& anim, double secondsTime)
{
    if (animations_.find(anim) != animations_.end())
    {
        double ticksPerSecond = animations_[anim]->ticksPerSecond != 0 ? animations_[anim]->ticksPerSecond : 25.0;
        double timeInTicks = secondsTime * ticksPerSecond;
        double animationTime = fmod(timeInTicks, animations_[anim]->duration);

        glm::mat4 rootTransform = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f));

        CalculateTransformsInHierarchy(anim, animationTime, skeleton_->rootJoint, rootTransform);
    }
}

void ZModelManager::CalculateTransformsInHierarchy(const std::string& animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform)
{
    std::shared_ptr<ZAnimation> animation = animations_[animName];
    glm::mat4 jointTransform = joint->transform;

    std::shared_ptr<ZJointAnimation> jointAnimation;
    for (unsigned int i = 0, j = animation->channels.size(); i < j; i++)
    {
        std::shared_ptr<ZJointAnimation> anim = animation->channels[i];
        if (anim->jointName == joint->name)
        {
            jointAnimation = anim; break;
        }
    }

    if (jointAnimation)
    {
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

    if (bonesMap_.find(joint->name) != bonesMap_.end())
    {
        unsigned int index = bonesMap_[joint->name];
        bones_[index]->transformation = globalInverseTransform_ * globalTransform * bones_[index]->offset;
        uniformBuffer_->Update(offsetof(ZModelUniforms, bones) + sizeof(glm::mat4) * index, sizeof(glm::mat4), glm::value_ptr(bones_[index]->transformation));
    }

    for (unsigned int i = 0; i < joint->children.size(); i++)
    {
        CalculateTransformsInHierarchy(animName, animTime, joint->children[i], globalTransform);
    }
}

glm::vec3 ZModelManager::CalculateInterpolatedScaling(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim)
{
    if (jointAnim->scalingKeys.size() == 1)
    {
        return jointAnim->scalingKeys[0].value;
    }

    unsigned int scalingIndex = 0;
    for (unsigned int i = 0, j = jointAnim->scalingKeys.size() - 1; i < j; i++)
    {
        if (animationTime < jointAnim->scalingKeys[i + 1].time)
        {
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

glm::quat ZModelManager::CalculateInterpolatedRotation(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim)
{
    if (jointAnim->rotationKeys.size() == 1)
    {
        return jointAnim->rotationKeys[0].value;
    }

    unsigned int rotationIndex = 0;
    for (unsigned int i = 0, j = jointAnim->rotationKeys.size() - 1; i < j; i++)
    {
        if (animationTime < jointAnim->rotationKeys[i + 1].time)
        {
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

glm::vec3 ZModelManager::CalculateInterpolatedPosition(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim)
{
    if (jointAnim->positionKeys.size() == 1)
    {
        return jointAnim->positionKeys[0].value;
    }

    unsigned int positionIndex = 0;
    for (unsigned int i = 0, j = jointAnim->positionKeys.size() - 1; i < j; i++)
    {
        if (animationTime < jointAnim->positionKeys[i + 1].time)
        {
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

void ZModelManager::HandleModelLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (event->Resource() == nullptr)
    {
        return;
    }

    std::shared_ptr<ZModelResourceData> modelResource = std::static_pointer_cast<ZModelResourceData>(event->Resource());

    if (modelResource->path == modelPath_)
    {
        meshes_ = modelResource->meshMap;
        bonesMap_ = modelResource->boneMap;
        bones_ = modelResource->boneList;
        animations_ = modelResource->animationMap;
        skeleton_ = modelResource->skeleton;

        for (ZMesh3DMap::iterator it = meshes_.begin(); it != meshes_.end(); it++)
            it->second->Initialize();
        if (skeleton_ && skeleton_->rootJoint)
            globalInverseTransform_ = glm::inverse(skeleton_->rootJoint->transform);

        ZServices::EventAgent()->Unsubscribe(this, &ZModel::HandleModelLoaded);

        Initialize();
    }
}


void ZModelManager::Track(const ZHModel& handle)
{
	ZModel* model = modelPool_.Get(handle);
	assert(model != nullptr && "Cannot track this model since it doesn't exist!");
	loadedModels_[model->name] = handle;
}
