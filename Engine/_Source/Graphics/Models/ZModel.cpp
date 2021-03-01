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

#include "ZServices.hpp"
#include "ZFramebuffer.hpp"
#include "ZPlane.hpp"
#include "ZCube.hpp"
#include "ZSphere.hpp"
#include "ZCylinder.hpp"
#include "ZShader.hpp"
#include "ZMaterial.hpp"
#include "ZModelImporter.hpp"
#include "ZAnimation.hpp"
#include "ZSkeleton.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"

ZIDSequence ZModel::idGenerator_("ZMOD");

ZModel::ZModel(const std::string& path)
    : modelPath_(path)
{
    id_ = std::to_string(idGenerator_.Next());
}

void ZModel::Initialize()
{
    if (!modelPath_.empty() && meshes_.empty())
    {
        ZModelImporter importer;
        meshes_ = importer.LoadModel(modelPath_, bonesMap_, bones_, animations_, skeleton_);
        for (ZMesh3DMap::iterator it = meshes_.begin(); it != meshes_.end(); it++) it->second->Initialize();
        if (skeleton_ && skeleton_->rootJoint) globalInverseTransform_ = glm::inverse(skeleton_->rootJoint->transform);
    }

    ComputeBounds();

    bool isRigged = skeleton_ != nullptr;
    uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::Model, sizeof(ZModelUniforms));
    uniformBuffer_->Update(offsetof(ZModelUniforms, rigged), sizeof(isRigged), &isRigged);

    if (skeleton_) {
        for (auto i = 0; i < bones_.size(); i++) {
            uniformBuffer_->Update(offsetof(ZModelUniforms, bones) + sizeof(glm::mat4) * i, sizeof(glm::mat4), glm::value_ptr(bones_[i]->transformation));
        }
    }

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();

    std::shared_ptr<ZModelReadyEvent> modelReadyEvent = std::make_shared<ZModelReadyEvent>(shared_from_this());
    ZServices::EventAgent()->Queue(modelReadyEvent);
}

void ZModel::Initialize(const std::shared_ptr<ZOFNode>& data)
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

void ZModel::InitializeAsync()
{
    ZResource modelResource(modelPath_, ZResourceType::Model);
    ZServices::ResourceCache()->RequestHandle(modelResource);

    ZServices::EventAgent()->Subscribe(this, &ZModel::HandleModelLoaded);
}

void ZModel::SetInstanceData(const ZInstancedDataOptions& instanceData)
{
    instanceData_ = instanceData;
    bool isInstanced = instanceData_.count > 1;
    uniformBuffer_->Update(offsetof(ZModelUniforms, instanced), sizeof(isInstanced), &isInstanced);
    for (auto it = meshes_.begin(); it != meshes_.end(); it++)
    {
        it->second->SetInstanceData(instanceData);
    }
}

void ZModel::BoneTransform(const std::string& anim, double secondsTime)
{
    glm::mat4 identity(1.f);

    if (animations_.find(anim) != animations_.end())
    {
        double ticksPerSecond = animations_[anim]->ticksPerSecond != 0 ? animations_[anim]->ticksPerSecond : 25.0;
        double timeInTicks = secondsTime * ticksPerSecond;
        double animationTime = fmod(timeInTicks, animations_[anim]->duration);

        CalculateTransformsInHierarchy(anim, animationTime, skeleton_->rootJoint, identity);
    }
}

void ZModel::ComputeBounds()
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

void ZModel::CalculateTransformsInHierarchy(const std::string& animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform)
{
    std::shared_ptr<ZAnimation> animation = animations_[animName];
    glm::mat4 jointTransform = joint->transform;

    if (joint->name.find("root") != std::string::npos)
    {
        jointTransform = glm::rotate(jointTransform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        jointTransform = glm::translate(jointTransform, glm::vec3(0.f, -0.337f, -0.175f));
    }

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

glm::vec3 ZModel::CalculateInterpolatedScaling(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim)
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

    return start + (float) factor * delta;
}

glm::quat ZModel::CalculateInterpolatedRotation(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim)
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
    glm::quat out = glm::mix(start, end, (float) factor);

    return glm::normalize(out);
}

glm::vec3 ZModel::CalculateInterpolatedPosition(double animationTime, std::shared_ptr<ZJointAnimation> jointAnim)
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

    return start + (float) factor * delta;
}

void ZModel::HandleModelLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (!event->Handle()) return;

    std::shared_ptr<ZModelResourceExtraData> extraData = std::static_pointer_cast<ZModelResourceExtraData>(event->Handle()->ExtraData());

    if (event->Handle()->Resource().name == modelPath_)
    {
        meshes_ = extraData->Meshes();
        bonesMap_ = extraData->BoneMap();
        bones_ = extraData->Bones();
        animations_ = extraData->Animations();
        skeleton_ = extraData->Skeleton();

        for (ZMesh3DMap::iterator it = meshes_.begin(); it != meshes_.end(); it++)
            it->second->Initialize();
        if (skeleton_ && skeleton_->rootJoint)
            globalInverseTransform_ = glm::inverse(skeleton_->rootJoint->transform);

        ZServices::EventAgent()->Unsubscribe(this, &ZModel::HandleModelLoaded);

        Initialize();
    }
}

void ZModel::Create(std::shared_ptr<ZOFTree> data, ZModelMap& outModelMap)
{
    ZModelMap models;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZMOD") == 0)
        {
            std::shared_ptr<ZOFObjectNode> modelNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (modelNode->properties.find("type") != modelNode->properties.end())
            {
                std::shared_ptr<ZOFString> typeProp = modelNode->properties["type"]->Value<ZOFString>(0);
                std::shared_ptr<ZModel> model = ZModel::Create(typeProp->value, it->second);
                model->id_ = it->first;
                models[model->id_] = model;
            }
            else if (modelNode->properties.find("path") != modelNode->properties.end()) {
                std::shared_ptr<ZOFString> pathProp = modelNode->properties["path"]->Value<ZOFString>(0);
                std::shared_ptr<ZModel> model = ZModel::CreateExternal(pathProp->value);
                model->id_ = it->first;
                models[model->id_] = model;
            }
        }
    }
    outModelMap = models;
}

void ZModel::CreateAsync(std::shared_ptr<ZOFTree> data, ZModelIDMap& outPendingModels, ZModelMap& outModelMap)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZMOD") == 0)
        {
            std::string path;
            std::shared_ptr<ZOFObjectNode> modelNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (modelNode->properties.find("type") != modelNode->properties.end())
            {
                std::shared_ptr<ZOFString> typeProp = modelNode->properties["type"]->Value<ZOFString>(0);
                std::shared_ptr<ZModel> model = ZModel::Create(typeProp->value, it->second);
                model->id_ = it->first;
                outModelMap[model->id_] = model;
            }
            if (modelNode->properties.find("path") != modelNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = modelNode->properties["path"]->Value<ZOFString>(0);
                std::shared_ptr<ZModel> model = ZModel::CreateExternal(pathStr->value, true);
                model->id_ = it->first;
                outPendingModels[model] = it->first;
            }
        }
    }
}

std::shared_ptr<ZModel> ZModel::Create(const std::string& type, const std::shared_ptr<ZOFNode>& data)
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

std::shared_ptr<ZModel> ZModel::CreateExternal(const std::string& path, bool async)
{
    std::shared_ptr<ZModel> model = std::make_shared<ZModel>(path);
    if (async)
        model->InitializeAsync();
    else
        model->Initialize();
    return model;
}
