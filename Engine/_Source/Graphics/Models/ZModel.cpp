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
#include "ZCompositeModel.hpp"
#include "ZMaterial.hpp"
#include "ZModelImporter.hpp"
#include "ZAnimation.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"
#include "ZServices.hpp"

ZModelType ModelTypeFromString(const std::string& type)
{
    if (type == "Plane")
    {
        return ZModelType::Plane;
    }
    else if (type == "Cube")
    {
        return ZModelType::Cube;
    }
    else if (type == "Sphere")
    {
        return ZModelType::Sphere;
    }
	else if (type == "Cylinder")
	{
        return ZModelType::Cylinder;
	}
	else if (type == "Cone")
	{
        return ZModelType::Cone;
	}
    return ZModelType::Custom;
}

ZIDSequence ZModel::idGenerator_;

ZModel::ZModel()
{
    name = "Model_" + std::to_string(idGenerator_.Next());
}

void ZModelManager::Initialize()
{
    ZServices::EventAgent()->Subscribe(this, &ZModelManager::HandleModelLoaded);
}

void ZModelManager::CleanUp()
{
    ZServices::EventAgent()->Unsubscribe(this, &ZModelManager::HandleModelLoaded);
}

ZHModel ZModelManager::Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
    if (dataNode->type != ZOFObjectType::Model)
    {
        return ZHModel();
    }

    ZHModel restoreHandle(dataHandle.value);

    if (dataNode->properties.find("type") != dataNode->properties.end())
    {
        std::shared_ptr<ZOFString> typeProp = dataNode->properties["type"]->Value<ZOFString>(0);
        ZHModel handle = Create(ModelTypeFromString(typeProp->value), restoreHandle);
        Dereference(handle)->OnDeserialize(dataNode);
        return handle;
    }
    else if (dataNode->properties.find("path") != dataNode->properties.end()) {
        std::shared_ptr<ZOFString> pathProp = dataNode->properties["path"]->Value<ZOFString>(0);
        ZHModel handle = Create(pathProp->value, restoreHandle);
        Dereference(handle)->OnDeserialize(dataNode);
        return handle;
	}

    return ZHModel();
}

void ZModelManager::DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
	if (dataNode->type != ZOFObjectType::Model)
	{
		return;
	}

	ZHModel restoreHandle(dataHandle.value);

	if (dataNode->properties.find("type") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> typeProp = dataNode->properties["type"]->Value<ZOFString>(0);
        ZHModel handle = Create(ModelTypeFromString(typeProp->value), restoreHandle);
        Dereference(handle)->OnDeserialize(dataNode);
    }
    if (dataNode->properties.find("path") != dataNode->properties.end())
    {
        std::shared_ptr<ZOFString> pathStr = dataNode->properties["path"]->Value<ZOFString>(0);
        CreateAsync(pathStr->value, restoreHandle);
	}
}

ZHModel ZModelManager::Create(const ZModelType& type, const ZHModel& restoreHandle, const std::shared_ptr<ZOFObjectNode>& deserializedNode)
{
    ZHModel handle(restoreHandle);
	ZModel* model = nullptr;

    bool isDeserialized = !handle.IsNull();

    switch (type)
    {
        case ZModelType::Plane:
            model = resourcePool_.New<ZPlane>(handle);
            break;
        case ZModelType::Cube:
			model = resourcePool_.New<ZCube>(handle);
            break;
        case ZModelType::Sphere:
			model = resourcePool_.New<ZSphere>(handle);
            break;
        case ZModelType::Cylinder:
			model = resourcePool_.New<ZCylinder>(handle);
            break;
        case ZModelType::Composite:
            model = resourcePool_.New<ZCompositeModel>(handle);
            break;
        case ZModelType::Custom:
            model = resourcePool_.New(handle);
        case ZModelType::Cone:
        default: break;
    }

    if (model != nullptr)
    {
        if (isDeserialized)
        {
            model->OnDeserialize(deserializedNode);
        }
        else
        {
            model->OnCreate();
        }

		ComputeBounds(handle);

		bool isRigged = false;
        model->uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Model, sizeof(ZModelUniforms));
        model->uniformBuffer->Update(offsetof(ZModelUniforms, rigged), sizeof(isRigged), &isRigged);

		ZRenderStateGroupWriter writer;
		writer.Begin();
		writer.BindUniformBuffer(model->uniformBuffer);
        model->renderState = writer.End();

		std::shared_ptr<ZModelReadyEvent> modelReadyEvent = std::make_shared<ZModelReadyEvent>(handle);
		ZServices::EventAgent()->Queue(modelReadyEvent);
    }
    else
    {
        LOG("Model type is not supported.", ZSeverity::Warning);
    }

    return handle;
}

ZHModel ZModelManager::Create(const std::string& path, const ZHModel& restoreHandle, const std::shared_ptr<ZOFObjectNode>& deserializedNode)
{
    if (path.empty())
    {
        return ZHModel();
    }

	ZHModel handle(restoreHandle);

    bool isDeserialized = !handle.IsNull();

	ZModel* model = resourcePool_.New(handle);

	ZModelImporter importer;
    model->meshes = importer.LoadModel(path, model->bonesMap, model->bones, model->animations, model->skeleton);

	for (ZMesh3DList::iterator it = model->meshes.begin(); it != model->meshes.end(); it++)
    {
        it->Initialize();
    }

	if (isDeserialized)
	{
		model->OnDeserialize(deserializedNode);
	}
	else
	{
		model->OnCreate();
	}

	if (model->skeleton.rootJoint)
    {
        model->globalInverseTransform = glm::inverse(model->skeleton.rootJoint->transform);
    }

	ComputeBounds(handle);

	bool isRigged = !model->bonesMap.empty();
    model->uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Model, sizeof(ZModelUniforms));
    model->uniformBuffer->Update(offsetof(ZModelUniforms, rigged), sizeof(isRigged), &isRigged);

	for (auto i = 0; i < model->bones.size(); i++) {
        model->uniformBuffer->Update(offsetof(ZModelUniforms, bones) + sizeof(glm::mat4) * i, sizeof(glm::mat4), glm::value_ptr(model->bones[i].transformation));
	}

	ZRenderStateGroupWriter writer;
	writer.Begin();
	writer.BindUniformBuffer(model->uniformBuffer);
    model->renderState = writer.End();


	std::shared_ptr<ZModelReadyEvent> modelReadyEvent = std::make_shared<ZModelReadyEvent>(handle);
	ZServices::EventAgent()->Queue(modelReadyEvent);

    return handle;
}

void ZModelManager::CreateAsync(const std::string& path, const ZHModel& restoreHandle, const std::shared_ptr<ZOFObjectNode>& deserializedNode)
{
	if (path.empty())
	{
		return;
	}

	ZHModel handle(restoreHandle);

	ZModel* model = resourcePool_.New(handle);

	ZModelImporter importer;
	model->meshes = importer.LoadModel(path, model->bonesMap, model->bones, model->animations, model->skeleton);

    if (!handle.IsNull())
    {
        asyncDeserializeDataNodes_[handle.Handle()] = deserializedNode;
    }
}

const std::string& ZModelManager::Name(const ZHModel& handle)
{
	assert(!handle.IsNull() && "Cannot use model with a null model handle!");
	ZModel* model = resourcePool_.Get(handle);
    return model->name;
}

const std::string& ZModelManager::Path(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
    return model->path;
}

const ZMesh3DList& ZModelManager::Meshes(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
    return model->meshes;
}

const ZBoneList& ZModelManager::Bones(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
	return model->bones;
}

const ZBoneMap& ZModelManager::BonesMap(const ZHModel& handle)
{
	assert(!handle.IsNull() && "Cannot use model with a null model handle!");
	ZModel* model = resourcePool_.Get(handle);
    return model->bonesMap;
}

const ZAnimationMap& ZModelManager::Animations(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
    return model->animations;
}

const ZInstancedDataOptions& ZModelManager::InstanceData(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
    return model->instanceData;
}

const ZSkeleton ZModelManager::Skeleton(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
    return model->skeleton;
}

const glm::mat4 ZModelManager::GlobalInverseTransform(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
    return model->globalInverseTransform;
}

const ZAABBox& ZModelManager::Bounds(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
    return model->bounds;
}

const std::shared_ptr<ZRenderStateGroup> ZModelManager::RenderState(const ZHModel& handle)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);
	return model->renderState;
}

void ZModelManager::SetInstanceData(const ZHModel& handle, const ZInstancedDataOptions& instanceData)
{
	assert(!handle.IsNull() && "Cannot use model with a null model handle!");
	ZModel* model = resourcePool_.Get(handle);

    model->instanceData = instanceData;
    bool isInstanced = model->instanceData.count > 1;
    model->uniformBuffer->Update(offsetof(ZModelUniforms, instanced), sizeof(isInstanced), &isInstanced);
    for (auto it = model->meshes.begin(); it != model->meshes.end(); it++)
    {
        it->SetInstanceData(instanceData);
    }
}

void ZModelManager::BoneTransform(const ZHModel& handle, const std::string& anim, double secondsTime)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);

    if (model->animations.find(anim) != model->animations.end())
    {
        double ticksPerSecond = model->animations[anim].ticksPerSecond != 0 ? model->animations[anim].ticksPerSecond : 25.0;
        double timeInTicks = secondsTime * ticksPerSecond;
        double animationTime = fmod(timeInTicks, model->animations[anim].duration);

        glm::mat4 rootTransform = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f));

        CalculateTransformsInHierarchy(handle, anim, animationTime, model->skeleton.rootJoint, rootTransform);
    }
}

void ZModelManager::ComputeBounds(const ZHModel& handle)
{
	assert(!handle.IsNull() && "Cannot use model with a null model handle!");
	ZModel* model = resourcePool_.Get(handle);

    model->bounds = ZAABBox();

    auto it = model->meshes.cbegin(), end = model->meshes.cend();
    for (; it != end; it++)
    {
        const ZVertex3DList& vertices = it->Vertices();
        for (int i = 0; i < vertices.size(); i++)
        {
            model->bounds = ZAABBox::Union(model->bounds, vertices[i].position);
        }
    }
}

void ZModelManager::CalculateTransformsInHierarchy(const ZHModel& handle, const std::string& animName, double animTime, const std::shared_ptr<ZJoint> joint, const glm::mat4& parentTransform)
{
    assert(!handle.IsNull() && "Cannot use model with a null model handle!");
    ZModel* model = resourcePool_.Get(handle);

    ZAnimation& animation = model->animations[animName];
    glm::mat4 jointTransform = joint->transform;

    ZJointAnimation jointAnimation;
    for (unsigned int i = 0, j = animation.channels.size(); i < j; i++)
    {
        ZJointAnimation anim = animation.channels[i];
        if (anim.jointName == joint->name)
        {
            jointAnimation = anim; break;
        }
    }

    if (jointAnimation.IsValid())
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

    if (model->bonesMap.find(joint->name) != model->bonesMap.end())
    {
        unsigned int index = model->bonesMap[joint->name];
        model->bones[index].transformation = model->globalInverseTransform * globalTransform * model->bones[index].offset;
        model->uniformBuffer->Update(offsetof(ZModelUniforms, bones) + sizeof(glm::mat4) * index, sizeof(glm::mat4), glm::value_ptr(model->bones[index].transformation));
    }

    for (unsigned int i = 0; i < joint->children.size(); i++)
    {
        CalculateTransformsInHierarchy(handle, animName, animTime, joint->children[i], globalTransform);
    }
}

glm::vec3 ZModelManager::CalculateInterpolatedScaling(double animationTime, const ZJointAnimation& jointAnim)
{
    if (jointAnim.scalingKeys.size() == 1)
    {
        return jointAnim.scalingKeys[0].value;
    }

    unsigned int scalingIndex = 0;
    for (unsigned int i = 0, j = jointAnim.scalingKeys.size() - 1; i < j; i++)
    {
        if (animationTime < jointAnim.scalingKeys[i + 1].time)
        {
            scalingIndex = i; break;
        }
    }
    unsigned int nextScalingIndex = scalingIndex + 1;
    double deltaTime = jointAnim.scalingKeys[nextScalingIndex].time - jointAnim.scalingKeys[scalingIndex].time;
    double factor = (animationTime - jointAnim.scalingKeys[scalingIndex].time) / deltaTime;
    glm::vec3 start = jointAnim.scalingKeys[scalingIndex].value;
    glm::vec3 end = jointAnim.scalingKeys[nextScalingIndex].value;
    glm::vec3 delta = end - start;

    return start + (float)factor * delta;
}

glm::quat ZModelManager::CalculateInterpolatedRotation(double animationTime, const ZJointAnimation& jointAnim)
{
    if (jointAnim.rotationKeys.size() == 1)
    {
        return jointAnim.rotationKeys[0].value;
    }

    unsigned int rotationIndex = 0;
    for (unsigned int i = 0, j = jointAnim.rotationKeys.size() - 1; i < j; i++)
    {
        if (animationTime < jointAnim.rotationKeys[i + 1].time)
        {
            rotationIndex = i; break;
        }
    }
    unsigned int nextRotationIndex = rotationIndex + 1;
    double deltaTime = jointAnim.rotationKeys[nextRotationIndex].time - jointAnim.rotationKeys[rotationIndex].time;
    double factor = (animationTime - jointAnim.rotationKeys[rotationIndex].time) / deltaTime;
    glm::quat start = jointAnim.rotationKeys[rotationIndex].value;
    glm::quat end = jointAnim.rotationKeys[nextRotationIndex].value;
    glm::quat out = glm::mix(start, end, (float)factor);

    return glm::normalize(out);
}

glm::vec3 ZModelManager::CalculateInterpolatedPosition(double animationTime, const ZJointAnimation& jointAnim)
{
    if (jointAnim.positionKeys.size() == 1)
    {
        return jointAnim.positionKeys[0].value;
    }

    unsigned int positionIndex = 0;
    for (unsigned int i = 0, j = jointAnim.positionKeys.size() - 1; i < j; i++)
    {
        if (animationTime < jointAnim.positionKeys[i + 1].time)
        {
            positionIndex = i; break;
        }
    }
    unsigned int nextPositionIndex = positionIndex + 1;
    double deltaTime = jointAnim.positionKeys[nextPositionIndex].time - jointAnim.positionKeys[positionIndex].time;
    double factor = (animationTime - jointAnim.positionKeys[positionIndex].time) / deltaTime;
    glm::vec3 start = jointAnim.positionKeys[positionIndex].value;
    glm::vec3 end = jointAnim.positionKeys[nextPositionIndex].value;
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

	ZHModel handle(modelResource->restoreHandle);

	ZModel* model = resourcePool_.New(handle);

    if (model)
    {
        model->path = modelResource->path;
        model->meshes = modelResource->meshMap;
        model->bonesMap = modelResource->boneMap;
        model->bones = modelResource->boneList;
        model->animations = modelResource->animationMap;
        model->skeleton = modelResource->skeleton;

        if (asyncDeserializeDataNodes_.find(handle.Handle()) != asyncDeserializeDataNodes_.end())
        {
            model->OnDeserialize(asyncDeserializeDataNodes_[handle.Handle()]);
            asyncDeserializeDataNodes_.erase(handle.Handle());
        }
        else
        {
            model->OnCreate();
        }

		for (ZMesh3DList::iterator it = model->meshes.begin(); it != model->meshes.end(); it++)
		{
			it->Initialize();
		}

		if (model->skeleton.rootJoint)
		{
			model->globalInverseTransform = glm::inverse(model->skeleton.rootJoint->transform);
		}

		ComputeBounds(handle);

		bool isRigged = !model->bonesMap.empty();
		model->uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Model, sizeof(ZModelUniforms));
		model->uniformBuffer->Update(offsetof(ZModelUniforms, rigged), sizeof(isRigged), &isRigged);

		for (auto i = 0; i < model->bones.size(); i++) {
			model->uniformBuffer->Update(offsetof(ZModelUniforms, bones) + sizeof(glm::mat4) * i, sizeof(glm::mat4), glm::value_ptr(model->bones[i].transformation));
		}

		ZRenderStateGroupWriter writer;
		writer.Begin();
		writer.BindUniformBuffer(model->uniformBuffer);
		model->renderState = writer.End();

        model->OnCreate();

		std::shared_ptr<ZModelReadyEvent> modelReadyEvent = std::make_shared<ZModelReadyEvent>(handle);
		ZServices::EventAgent()->Queue(modelReadyEvent);
    }
}