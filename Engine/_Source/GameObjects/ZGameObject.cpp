/*
   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZGameObject.cpp

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

#include "ZGameObject.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZLight.hpp"
#include "ZSkybox.hpp"
#include "ZGrass.hpp"
#include "ZCamera.hpp"
#include "ZParticle.hpp"
#include "ZSceneRoot.hpp"
#include "ZGame.hpp"
#include "ZScene.hpp"
#include "ZModel.hpp"
#include "ZFrustum.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZScriptComponent.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZAnimatorComponent.hpp"
#include "ZRigidBody.hpp"
#include "ZOFTree.hpp"
#include "ZIDSequence.hpp"
#include "ZObjectDestroyedEvent.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"

ZGameObjectType ObjectTypeFromString(const std::string& str)
{
    if (str == "Camera")
    {
        return ZGameObjectType::Camera;
    }
    else if (str == "Grass")
    {
        return ZGameObjectType::Grass;
    }
    else if (str == "Light")
    {
        return ZGameObjectType::Light;
    }
    else if (str == "Particle")
    {
        return ZGameObjectType::Particle;
    }
    else if (str == "SceneRoot")
    {
        return ZGameObjectType::SceneRoot;
    }
    else if (str == "Skybox")
    {
        return ZGameObjectType::Skybox;
    }
    return ZGameObjectType::Custom;
}

ZIDSequence ZGameObject::idGenerator_;

ZGameObject::ZGameObject()
{
    properties.modelMatrix = properties.localModelMatrix = glm::mat4(1.f);
    properties.renderOrder = ZRenderLayer::Static;
    name = "GameObject" + std::to_string(idGenerator_.Next());
}

ZGameObject::ZGameObject(const ZGameObject& other)
{
	handle = other.handle;
	name = other.name;
    parent = other.parent;
    components = other.components;
    children = other.children;
    properties = other.properties;
    renderState = other.renderState;
    uniformBuffer = other.uniformBuffer;
}

ZGameObject& ZGameObject::operator=(const ZGameObject& other)
{
	handle = other.handle;
	name = other.name;
	parent = other.parent;
	components = other.components;
	children = other.children;
	properties = other.properties;
	renderState = other.renderState;
	uniformBuffer = other.uniformBuffer;
    return *this;
}

ZHGameObject ZGameObjectManager::Create(const ZGameObjectType& type, const ZHGameObject& restoreHandle)
{
    ZHGameObject handle(restoreHandle);
    ZGameObject* object = nullptr;

    switch (type)
    {
    case ZGameObjectType::Custom:
        object = resourcePool_.New<ZGameObject>(handle);
        break;
    case ZGameObjectType::Camera:
        object = resourcePool_.New<ZCamera>(handle);
        break;
    case ZGameObjectType::Grass:
        object = resourcePool_.New<ZGrass>(handle);
        break;
    case ZGameObjectType::Light:
        object = resourcePool_.New<ZLight>(handle);
        break;
    case ZGameObjectType::Particle:
        object = resourcePool_.New<ZParticle>(handle);
        break;
    case ZGameObjectType::SceneRoot:
        object = resourcePool_.New<ZSceneRoot>(handle);
        break;
    case ZGameObjectType::Skybox:
        object = resourcePool_.New<ZSkybox>(handle);
        break;
    default: break;
    }

    if (object != nullptr)
    {
        object->type = type;
    }

    return handle;
}

ZHGameObject ZGameObjectManager::CreateReady(const ZGameObjectType& type, const std::shared_ptr<ZScene>& scene, const ZHGameObject& restoreHandle)
{
    ZHGameObject handle = Create(type, restoreHandle);

    if (!handle.IsNull())
    {
        ZGameObject* object = resourcePool_.Get(handle);
        object->handle = handle;
        object->scene = scene;

        CalculateDerivedData(handle);
        object->OnCreate();

        // Give subclasses a chance to create custom uniform buffers before creating a default
        if (object->uniformBuffer == nullptr)
        {
            object->uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Object, sizeof(ZObjectUniforms));
        }

        ZRenderStateGroupWriter writer;
        writer.Begin();
        writer.BindUniformBuffer(object->uniformBuffer);
        object->renderState = writer.End();
    }

    return handle;
}

ZHGameObject ZGameObjectManager::Deserialize(const ZOFHandle& dataHandle, const std::shared_ptr<ZOFObjectNode>& dataNode, const std::shared_ptr<ZScene>& scene)
{
    if (dataNode == nullptr)
    {
        LOG("Could not initalize ZGameObject", ZSeverity::Error);
        return ZHGameObject();
    }

    ZHGameObject handle(dataHandle.value);

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("type") != props.end() && props["type"]->HasValues())
    {
        std::shared_ptr<ZOFString> typeProp = props["type"]->Value<ZOFString>(0);
        handle = Create(ObjectTypeFromString(typeProp->value), handle);
    }

    if (handle.IsNull())
    {
        return handle;
    }

    ZGameObject* object = resourcePool_.Get(handle);

    if (props.find("position") != props.end() && props["position"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> posProp = props["position"]->Value<ZOFNumberList>(0);
        object->properties.previousPosition = object->properties.position = glm::vec3(posProp->value[0], posProp->value[1], posProp->value[2]);
    }

    if (props.find("position") != props.end() && props["position"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> posProp = props["position"]->Value<ZOFNumberList>(0);
        object->properties.previousPosition = object->properties.position = glm::vec3(posProp->value[0], posProp->value[1], posProp->value[2]);
    }

    if (props.find("orientation") != props.end() && props["orientation"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> ornProp = props["orientation"]->Value<ZOFNumberList>(0);
        object->properties.previousOrientation = object->properties.orientation = glm::quat(glm::vec3(glm::radians(ornProp->value[0]), glm::radians(ornProp->value[1]), glm::radians(ornProp->value[2])));
    }

    if (props.find("scale") != props.end() && props["scale"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> scaleProp = props["scale"]->Value<ZOFNumberList>(0);
        object->properties.previousScale = object->properties.scale = glm::vec3(scaleProp->value[0], scaleProp->value[1], scaleProp->value[2]);
    }

    object->handle = handle;
    object->scene = scene;
    object->uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Object, sizeof(ZObjectUniforms));

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.BindUniformBuffer(object->uniformBuffer);
    object->renderState = writer.End();

    CalculateDerivedData(handle);
    object->OnDeserialize(dataNode);

    return handle;
}

void ZGameObjectManager::Update(const ZHGameObject& handle, double deltaTime)
{
    auto scene = Scene(handle);
    if (scene == nullptr)
    {   
        return;
    }

    ZGameObject* object = Dereference<ZGameObject>(handle);

    // TODO: Separate into tick buckets for different stages of ticking
    for (auto comp : object->components)
    {
        if (ZGraphicsComponent* graphicsComp = ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(comp))
        {
            graphicsComp->gameLights = scene->GameLights();
            graphicsComp->gameCamera = scene->ActiveCamera();
        }
        ZAssets::ComponentManager()->Update(comp, deltaTime);
    }

    object->OnUpdate(deltaTime);

    UpdateChildren(handle, deltaTime);
}

void ZGameObjectManager::UpdateChildren(const ZHGameObject& handle, double deltaTime)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    ZGameObjectList::iterator it = gameObject->children.begin(), end = gameObject->children.end();
    for (; it != end; it++)
    {
        const ZHGameObject child = *it;
        if (IsVisible(child))
        {
            Update(child, deltaTime);
        }
        UpdateChildren(child, deltaTime);
    }

    gameObject->OnUpdateChildren(deltaTime);
}

void ZGameObjectManager::CalculateDerivedData(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->objectMutexes.position.lock();
    glm::mat4 translation = glm::translate(glm::mat4(1.f), gameObject->properties.position);
    gameObject->objectMutexes.position.unlock();

    gameObject->objectMutexes.orientation.lock();
    gameObject->properties.orientation = glm::normalize(gameObject->properties.orientation);
    glm::mat4 rotation = glm::mat4_cast(gameObject->properties.orientation);
    gameObject->objectMutexes.orientation.unlock();

    gameObject->objectMutexes.scale.lock();
    glm::mat4 scale = glm::scale(glm::mat4(1.f), gameObject->properties.scale);
    gameObject->objectMutexes.scale.unlock();

    SetLocalModelMatrix(handle, translation * rotation * scale);
}

ZHGameObject ZGameObjectManager::Clone(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    ZHGameObject clone = CreateReady(gameObject->type, gameObject->scene.lock());
    ZGameObject* cloneObject = resourcePool_.Get(clone);
    cloneObject->properties = gameObject->properties;
    cloneObject->parent = gameObject->parent;
    CalculateDerivedData(clone);

    for (const ZHComponent& comp : gameObject->components)
    {
        ZHComponent compClone = ZAssets::ComponentManager()->Clone(comp);
        AddComponent(clone, compClone);
        if (ZPhysicsComponent* physicsComp = ZAssets::ComponentManager()->Dereference<ZPhysicsComponent>(compClone))
        {
            physicsComp->body->SetGameObject(clone);
        }
    }

    for (const ZHGameObject& object : gameObject->children)
    {
        AddChild(clone, Clone(object));
    }

    cloneObject->OnCloned(gameObject);

    return clone;
}

void ZGameObjectManager::AddChild(const ZHGameObject& handle, const ZHGameObject& child)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    if (std::find(gameObject->children.begin(), gameObject->children.end(), child) == gameObject->children.end())
    {
        ZGameObject* childObject = resourcePool_.Get(child);
        gameObject->children.push_back(child);

        if (const auto& parent = childObject->parent)
        {
            RemoveChild(parent, child);
        }
        childObject->parent = handle;

        if (auto scene = Scene(handle))
        {
            scene->AddGameObject(child);
        }

        if (!gameObject->properties.active)
        {
            SetActive(child, gameObject->properties.active);
        }

        SetModelMatrix(child, gameObject->properties.modelMatrix * childObject->properties.localModelMatrix);
    }
}

void ZGameObjectManager::RemoveChild(const ZHGameObject& handle, const ZHGameObject& child, bool recurse)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    ZGameObjectList::iterator it = std::find(gameObject->children.begin(), gameObject->children.end(), child);
    if (it != gameObject->children.end())
    {
        ZGameObject* childObject = resourcePool_.Get(child);
        childObject->parent = ZHGameObject();
        gameObject->children.erase(it);
    }

    if (recurse)
    {
        for (auto it = gameObject->children.begin(), end = gameObject->children.end(); it != end; it++)
        {
            RemoveChild(*it, child, recurse);
        }
    }
}

bool ZGameObjectManager::HasChildren(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);
    return gameObject->children.size() > 0;
}

bool ZGameObjectManager::IsVisible(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    auto scene = Scene(handle);
    if (scene == nullptr)
    {
        return false;
    }

    bool isObjectVisible = gameObject->properties.active && gameObject->properties.visible;

    ZCamera* activeCameraObject = Dereference<ZCamera>(scene->ActiveCamera());
    if (const ZHComponent& graphicsComp = FindComponent<ZGraphicsComponent>(handle))
    {
        ZGraphicsComponent* graphicsCompObj = ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(graphicsComp);
        bool isInCameraFrustum = activeCameraObject != nullptr && graphicsCompObj->IsVisible(activeCameraObject->frustum);
        return isInCameraFrustum && isObjectVisible;
    }

    return isObjectVisible;
}

void ZGameObjectManager::Destroy(const ZHGameObject& handle)
{
    assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
    ZGameObject* gameObject = resourcePool_.Get(handle);

    for (const ZHGameObject& child : gameObject->children)
    {
        Destroy(child);
    }
    for (const ZHComponent& comp : gameObject->components)
    {
        ZAssets::ComponentManager()->CleanUp(comp);
    }

    std::shared_ptr<ZObjectDestroyedEvent> destroyEvent(new ZObjectDestroyedEvent(handle));
    ZServices::EventAgent()->Trigger(destroyEvent);
}

bool ZGameObjectManager::Active(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

	return gameObject->properties.active;
}

std::shared_ptr<ZScene> ZGameObjectManager::Scene(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    return gameObject->scene.lock();
}

ZHGameObject ZGameObjectManager::Parent(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    return gameObject->parent;
}

std::string ZGameObjectManager::Name(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    return gameObject->name;
}

ZRenderLayer ZGameObjectManager::RenderLayer(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    return gameObject->properties.renderOrder;
}

const ZComponentList& ZGameObjectManager::Components(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    return gameObject->components;
}

const ZGameObjectList& ZGameObjectManager::Children(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    return gameObject->children;
}

glm::vec3 ZGameObjectManager::Position(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 pos;
    gameObject->objectMutexes.position.lock();
    pos = gameObject->properties.position;
    gameObject->objectMutexes.position.unlock();
    return pos;

}

glm::vec3 ZGameObjectManager::Scale(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 scale;
    gameObject->objectMutexes.scale.lock();
    scale = gameObject->properties.scale;
    gameObject->objectMutexes.scale.unlock();
    return scale;

}

glm::quat ZGameObjectManager::Orientation(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::quat orn;
    gameObject->objectMutexes.orientation.lock();
    orn = gameObject->properties.orientation;
    gameObject->objectMutexes.orientation.unlock();
    return orn;

}

glm::vec3 ZGameObjectManager::Front(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 front;
    gameObject->objectMutexes.orientation.lock();
    front = glm::conjugate(gameObject->properties.orientation) * glm::vec3(0.f, 0.f, -1.f);
    gameObject->objectMutexes.orientation.unlock();
    return front;

}

glm::vec3 ZGameObjectManager::Up(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 up;
    gameObject->objectMutexes.orientation.lock();
    up = glm::conjugate(gameObject->properties.orientation) * glm::vec3(0.f, 1.f, 0.f);
    gameObject->objectMutexes.orientation.unlock();
    return up;
}

glm::vec3 ZGameObjectManager::Right(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 right;
    gameObject->objectMutexes.orientation.lock();
    right = glm::conjugate(gameObject->properties.orientation) * glm::vec3(-1.f, 0.f, 0.f);
    gameObject->objectMutexes.orientation.unlock();
    return right;

}

glm::mat4 ZGameObjectManager::ModelMatrix(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::mat4 M;
    gameObject->objectMutexes.modelMatrix.lock();
    M = gameObject->properties.modelMatrix;
    gameObject->objectMutexes.modelMatrix.unlock();
    return M;

}

glm::vec3 ZGameObjectManager::PreviousPosition(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 prev;
    gameObject->objectMutexes.position.lock();
    prev = glm::vec3(gameObject->properties.previousPosition);
    gameObject->objectMutexes.position.unlock();
    return prev;
}

glm::vec3 ZGameObjectManager::PreviousFront(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 prev;
    gameObject->objectMutexes.orientation.lock();
    prev = glm::conjugate(gameObject->properties.previousOrientation) * glm::vec3(0.f, 0.f, -1.f);
    gameObject->objectMutexes.orientation.unlock();
    return prev;
}

glm::vec3 ZGameObjectManager::PreviousUp(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 prev;
    gameObject->objectMutexes.orientation.lock();
    prev = glm::conjugate(gameObject->properties.previousOrientation) * glm::vec3(0.f, 1.f, 0.f);
    gameObject->objectMutexes.orientation.unlock();
    return prev;
}

glm::vec3 ZGameObjectManager::PreviousRight(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    glm::vec3 prev;
    gameObject->objectMutexes.orientation.lock();
    prev = glm::conjugate(gameObject->properties.previousOrientation) * glm::vec3(-1.f, 0.f, 0.f);
    gameObject->objectMutexes.orientation.unlock();
    return prev;
}

std::shared_ptr<ZRenderStateGroup> ZGameObjectManager::RenderState(const ZHGameObject& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    return gameObject->renderState;
}

void ZGameObjectManager::SetParent(const ZHGameObject& handle, const ZHGameObject& parent)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->parent = parent;
}

void ZGameObjectManager::SetScene(const ZHGameObject& handle, const std::shared_ptr<ZScene>& scene)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    if (!scene)
    {
        gameObject->scene.reset();
    }
    else
    {
        gameObject->scene = scene;
    }

    for (const ZHGameObject& child : gameObject->children)
    {
        SetScene(child, scene);
    }
}

void ZGameObjectManager::SetPosition(const ZHGameObject& handle, const glm::vec3& position)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->objectMutexes.position.lock();
    gameObject->properties.previousPosition = gameObject->properties.position;
    gameObject->properties.position = position;
    gameObject->objectMutexes.position.unlock();

    CalculateDerivedData(handle);
}

void ZGameObjectManager::SetScale(const ZHGameObject& handle, const glm::vec3& scale)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->objectMutexes.scale.lock();
    gameObject->properties.previousScale = gameObject->properties.scale;
    gameObject->properties.scale = scale;
    gameObject->objectMutexes.scale.unlock();

    CalculateDerivedData(handle);
}

void ZGameObjectManager::SetOrientation(const ZHGameObject& handle, const glm::quat& quaternion)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->objectMutexes.orientation.lock();
    gameObject->properties.previousOrientation = gameObject->properties.orientation;
    gameObject->properties.orientation = quaternion;
    gameObject->objectMutexes.orientation.unlock();

    CalculateDerivedData(handle);
}

void ZGameObjectManager::SetOrientation(const ZHGameObject& handle, const glm::vec3& euler)
{
    assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
    ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->objectMutexes.orientation.lock();
    gameObject->properties.previousOrientation = gameObject->properties.orientation;
    gameObject->properties.orientation = glm::quat(euler);
    gameObject->objectMutexes.orientation.unlock();

    CalculateDerivedData(handle);
}

void ZGameObjectManager::SetLocalModelMatrix(const ZHGameObject& handle, const glm::mat4& modelMatrix)
{
    assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
    ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->objectMutexes.localModelMatrix.lock();
    gameObject->properties.localModelMatrix = modelMatrix;
    gameObject->objectMutexes.localModelMatrix.unlock();

    if (!gameObject->parent.IsNull())
    {
        SetModelMatrix(handle, ModelMatrix(gameObject->parent) * gameObject->properties.localModelMatrix);
    }
    else
    {
        SetModelMatrix(handle, gameObject->properties.localModelMatrix);
    }
}

void ZGameObjectManager::SetModelMatrix(const ZHGameObject& handle, const glm::mat4& modelMatrix)
{
    assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
    ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->objectMutexes.modelMatrix.lock();
    gameObject->properties.modelMatrix = modelMatrix;
    gameObject->objectMutexes.modelMatrix.unlock();

    if (gameObject->uniformBuffer)
    {
        gameObject->uniformBuffer->Update(offsetof(ZObjectUniforms, M), sizeof(glm::mat4), glm::value_ptr(gameObject->properties.modelMatrix));
    }

    if (const ZHComponent& graphicsComp = FindComponent<ZGraphicsComponent>(handle))
    {
        ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(graphicsComp)->Transform(gameObject->properties.modelMatrix);
    }

    if (const ZHComponent& physicsComp = FindComponent<ZPhysicsComponent>(handle))
    {
        ZAssets::ComponentManager()->Dereference<ZPhysicsComponent>(physicsComp)->SetTransform(gameObject->properties.modelMatrix);
    }

    for (const ZHGameObject& child : gameObject->children)
    {
        ZGameObject* childObject = resourcePool_.Get(child);
        SetModelMatrix(child, gameObject->properties.modelMatrix * childObject->properties.localModelMatrix);
    }
}

void ZGameObjectManager::SetRenderOrder(const ZHGameObject& handle, ZRenderLayer renderOrder)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->properties.renderOrder = renderOrder;
}

void ZGameObjectManager::SetName(const ZHGameObject& handle, const std::string& name)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->name = name;
}

void ZGameObjectManager::SetActive(const ZHGameObject& handle, bool active)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    gameObject->properties.active = active;
    for (const ZHGameObject& child : gameObject->children)
    {
        SetActive(child, active);
    }
}

void ZGameObjectManager::Translate(const ZHGameObject& handle, const glm::vec3& translation, bool global)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

    if (global)
    {
        gameObject->objectMutexes.position.lock();
        gameObject->properties.previousPosition = gameObject->properties.position;
        gameObject->properties.position += translation;
        gameObject->objectMutexes.position.unlock();
    }
    else
    {
        gameObject->objectMutexes.position.lock();
        gameObject->properties.previousPosition = gameObject->properties.position;
        gameObject->properties.position += translation;
        gameObject->objectMutexes.position.unlock();
    }

    CalculateDerivedData(handle);
}

void ZGameObjectManager::AddComponent(const ZHGameObject& handle, const ZHComponent& component)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);
	ZComponent* compObjectToAdd = ZAssets::ComponentManager()->Dereference<ZComponent>(component);

    bool componentTypeExists = false;
    for (const ZHComponent& comp : gameObject->components)
    {
        ZComponent* compObjectToCheck = ZAssets::ComponentManager()->Dereference<ZComponent>(component);
		if (typeid(*compObjectToCheck) == typeid(*compObjectToAdd))
		{
			componentTypeExists = true;
			break;
		}
	}

	if (!componentTypeExists || ZComponent::MultipleSupported(compObjectToAdd->type))
	{
		compObjectToAdd->rootObject = handle;
		gameObject->components.push_back(component);
	}
}

ZHComponent ZGameObjectManager::RemoveComponent(const ZHGameObject& handle, const std::string& name)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

	if (!name.empty())
	{
		ZComponentList::iterator found = std::find_if(gameObject->components.begin(), gameObject->components.end(), [&name](const auto& comp) {
			ZComponent* compObj = ZAssets::ComponentManager()->Dereference<ZComponent>(comp);
			return compObj != nullptr && name == compObj->name;
			});

		if (found != gameObject->components.end())
		{
			ZHComponent removed = *found;
			gameObject->components.erase(found);
			return removed;
		}
	}
	return ZHComponent();
}

ZHComponent ZGameObjectManager::RemoveComponent(const ZHGameObject& handle, const ZHComponent& component)
{
	if (component.IsNull())
	{
		return ZHComponent();
	}

	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

	ZComponentList::iterator found = std::find_if(gameObject->components.begin(), gameObject->components.end(), [&component](const auto& comp) {
		return component == comp;
		});

	if (found != gameObject->components.end())
	{
		ZHComponent removed = *found;
		gameObject->components.erase(found);
		return removed;
	}

	return ZHComponent();
}

ZHGameObject ZGameObjectManager::FindChildByName(const ZHGameObject& handle, const std::string& name)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
	ZGameObject* gameObject = resourcePool_.Get(handle);

	for (auto it = gameObject->children.begin(); it != gameObject->children.end(); it++)
	{
		ZGameObject* childGameObject = resourcePool_.Get(*it);
		if (zenith::strings::HasSuffix(childGameObject->name, name))
		{
			return *it;
		}
	}

	return ZHGameObject();
}
