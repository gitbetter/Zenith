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

#include "ZServices.hpp"
#include "ZLight.hpp"
#include "ZSkybox.hpp"
#include "ZGrass.hpp"
#include "ZCamera.hpp"
#include "ZGame.hpp"
#include "ZScene.hpp"
#include "ZModel.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZScriptComponent.hpp"
#include "ZAnimatorComponent.hpp"
#include "ZRigidBody.hpp"
#include "ZOFTree.hpp"
#include "ZIDSequence.hpp"
#include "ZObjectDestroyedEvent.hpp"

ZIDSequence ZGameObject::idGenerator_("ZGO");

ZGameObject::ZGameObject(const glm::vec3& position, const glm::quat& orientation)
{
    properties_.previousPosition = properties_.position = glm::vec4(position, 1.f);
    properties_.previousScale = properties_.scale = glm::vec3(1.f, 1.f, 1.f);
    properties_.previousOrientation = properties_.orientation = orientation;
    properties_.modelMatrix = glm::mat4(1.f);
    properties_.renderOrder = ZRenderOrder::Static;
    id_ = "ZGO_" + idGenerator_.Next();
    CalculateDerivedData();
}

void ZGameObject::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZGameObject", ZSeverity::Error);
        return;
    }

    id_ = node->id;

    ZOFPropertyMap props = node->properties;

    if (props.find("position") != props.end() && props["position"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> posProp = props["position"]->Value<ZOFNumberList>(0);
        properties_.position = glm::vec4(posProp->value[0], posProp->value[1], posProp->value[2], 1.f);
        properties_.previousPosition = properties_.position;
    }

    if (props.find("orientation") != props.end() && props["orientation"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> ornProp = props["orientation"]->Value<ZOFNumberList>(0);
        properties_.orientation = glm::quat(glm::vec3(ornProp->value[0], ornProp->value[1], ornProp->value[2]));
        properties_.previousOrientation = properties_.orientation;
    }

    if (props.find("scale") != props.end() && props["scale"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> scaleProp = props["scale"]->Value<ZOFNumberList>(0);
        properties_.scale = glm::vec3(scaleProp->value[0], scaleProp->value[1], scaleProp->value[2]);
        properties_.previousScale = properties_.scale;
    }

    CalculateDerivedData();
}

void ZGameObject::PreRender()
{
    auto scene = Scene();
    if (!scene) return;

    scene->PushMatrix(scene->TopMatrix() * properties_.modelMatrix);
}

void ZGameObject::Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp)
{
    auto scene = Scene();
    if (!scene) return;

    if (std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>())
    {
        graphicsComp->SetGameLights(scene->GameLights());
        graphicsComp->SetGameCamera(scene->ActiveCamera());
        graphicsComp->Render(deltaTime, shader, renderOp);
    }
}

void ZGameObject::PostRender()
{
    auto scene = Scene();
    if (!scene) return;

    scene->PopMatrix();
}

void ZGameObject::RenderChildren(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp)
{
    ZGameObjectList::reverse_iterator it = children_.rbegin(), end = children_.rend();
    for (; it != end; it++)
    {
        auto go = *it;
        go->PreRender();
        if (go->IsVisible())
        {
            go->Render(deltaTime, shader, renderOp);
        }
        go->RenderChildren(deltaTime, shader, renderOp);
        go->PostRender();
    }
}

void ZGameObject::CalculateDerivedData()
{
    objectMutexes_.position.lock();
    glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(properties_.position));
    objectMutexes_.position.unlock();

    objectMutexes_.orientation.lock();
    properties_.orientation = glm::normalize(properties_.orientation);
    glm::mat4 rotation = glm::mat4_cast(properties_.orientation);
    objectMutexes_.orientation.unlock();

    objectMutexes_.scale.lock();
    glm::mat4 scale = glm::scale(glm::mat4(1.f), properties_.scale);
    objectMutexes_.scale.unlock();

    objectMutexes_.modelMatrix.lock();
    properties_.modelMatrix = translation * rotation * scale;
    objectMutexes_.modelMatrix.unlock();

    std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>();
    if (graphicsComp)
        graphicsComp->Model()->UpdateAABB(properties_.modelMatrix);
}

std::shared_ptr<ZGameObject> ZGameObject::Clone()
{
    std::shared_ptr<ZGameObject> clone = std::make_shared<ZGameObject>();
    clone->id_ = id_;
    clone->properties_ = properties_;
    clone->scene_ = scene_;
    clone->parent_ = parent_;

    // TODO: Can we template the component Clone method somehow?
    for (std::shared_ptr<ZComponent> comp : components_)
    {
        std::shared_ptr<ZComponent> compClone = comp->Clone();
        clone->AddComponent(compClone);
        if (auto physicsComp = std::dynamic_pointer_cast<ZPhysicsComponent>(compClone))
        {
            physicsComp->RigidBody()->SetGameObject(clone.get());
        }
    }

    for (std::shared_ptr<ZGameObject> object : children_)
        clone->AddChild(object->Clone());

    return clone;
}

void ZGameObject::AddChild(std::shared_ptr<ZGameObject> gameObject)
{
    if (std::find(children_.begin(), children_.end(), gameObject) == children_.end())
    {
        if (!gameObject->Scene()) gameObject->scene_ = scene_;
        if (auto parent = gameObject->Parent()) parent->RemoveChild(gameObject);
        gameObject->parent_ = shared_from_this();
        children_.push_back(gameObject);
    }
}

void ZGameObject::RemoveChild(std::shared_ptr<ZGameObject> gameObject, bool recurse)
{
    ZGameObjectList::iterator it = std::find(children_.begin(), children_.end(), gameObject);
    if (it != children_.end())
    {
        gameObject->parent_.reset();
        children_.erase(it);
    }

    if (recurse)
    {
        for (auto it = children_.begin(), end = children_.end(); it != end; it++)
        {
            (*it)->RemoveChild(gameObject, recurse);
        }
    }
}

bool ZGameObject::IsVisible()
{
    auto scene = Scene();
    if (!scene) return false;

    std::shared_ptr<ZCamera> activeCamera = scene->ActiveCamera();
    std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>();
    if (activeCamera && graphicsComp && graphicsComp->Model())
    {
        return activeCamera->Frustum().Contains(graphicsComp->Model()->AABB());
    }
    return false;
}

void ZGameObject::Destroy()
{
    CleanUp();

    for (auto object : children_)
        object->Destroy();
    for (auto comp : components_)
        comp->CleanUp();

    std::shared_ptr<ZObjectDestroyedEvent> destroyEvent(new ZObjectDestroyedEvent(shared_from_this()));
    ZServices::EventAgent()->Trigger(destroyEvent);
}

std::shared_ptr<ZScene> ZGameObject::Scene() const
{
    return scene_.lock();
}

std::shared_ptr<ZGameObject> ZGameObject::Parent() const
{
    return parent_.lock();
}

glm::vec3 ZGameObject::Position()
{
    glm::vec3 pos;
    objectMutexes_.position.lock();
    pos = glm::vec3(properties_.position);
    objectMutexes_.position.unlock();
    return pos;

}

glm::vec3 ZGameObject::Scale()
{
    glm::vec3 scale;
    objectMutexes_.scale.lock();
    scale = properties_.scale;
    objectMutexes_.scale.unlock();
    return scale;

}

glm::quat ZGameObject::Orientation()
{
    glm::quat orn;
    objectMutexes_.orientation.lock();
    orn = properties_.orientation;
    objectMutexes_.orientation.unlock();
    return orn;

}

glm::vec3 ZGameObject::Front()
{
    glm::vec3 front;
    objectMutexes_.orientation.lock();
    front = glm::conjugate(properties_.orientation) * glm::vec3(0.f, 0.f, -1.f);
    objectMutexes_.orientation.unlock();
    return front;

}

glm::vec3 ZGameObject::Up()
{
    glm::vec3 up;
    objectMutexes_.orientation.lock();
    up = glm::conjugate(properties_.orientation) * glm::vec3(0.f, 1.f, 0.f);
    objectMutexes_.orientation.unlock();
    return up;
}

glm::vec3 ZGameObject::Right()
{
    glm::vec3 right;
    objectMutexes_.orientation.lock();
    right = glm::conjugate(properties_.orientation) * glm::vec3(-1.f, 0.f, 0.f);
    objectMutexes_.orientation.unlock();
    return right;

}

glm::mat4 ZGameObject::ModelMatrix()
{
    glm::mat4 M;
    objectMutexes_.modelMatrix.lock();
    M = properties_.modelMatrix;
    objectMutexes_.modelMatrix.unlock();
    return M;

}

glm::vec3 ZGameObject::PreviousPosition()
{
    glm::vec3 prev;
    objectMutexes_.position.lock();
    prev = glm::vec3(properties_.previousPosition);
    objectMutexes_.position.unlock();
    return prev;
}

glm::vec3 ZGameObject::PreviousFront()
{
    glm::vec3 prev;
    objectMutexes_.orientation.lock();
    prev = glm::conjugate(properties_.previousOrientation) * glm::vec3(0.f, 0.f, -1.f);
    objectMutexes_.orientation.unlock();
    return prev;
}

glm::vec3 ZGameObject::PreviousUp()
{
    glm::vec3 prev;
    objectMutexes_.orientation.lock();
    prev = glm::conjugate(properties_.previousOrientation) * glm::vec3(0.f, 1.f, 0.f);
    objectMutexes_.orientation.unlock();
    return prev;
}

glm::vec3 ZGameObject::PreviousRight()
{
    glm::vec3 prev;
    objectMutexes_.orientation.lock();
    prev = glm::conjugate(properties_.previousOrientation) * glm::vec3(-1.f, 0.f, 0.f);
    objectMutexes_.orientation.unlock();
    return prev;
}

void ZGameObject::SetPosition(const glm::vec3& position)
{
    objectMutexes_.position.lock();
    properties_.previousPosition = properties_.position;
    properties_.position = glm::vec4(position, 1.f);
    objectMutexes_.position.unlock();

    std::shared_ptr<ZPhysicsComponent> physicsComp = FindComponent<ZPhysicsComponent>();
    if (physicsComp)
        physicsComp->RigidBody()->SetPosition(properties_.position);

    CalculateDerivedData();
}

void ZGameObject::SetScale(const glm::vec3& scale)
{
    // TODO: set the btRigidBody local scaling in the
    // physics component, if there is one
    objectMutexes_.scale.lock();
    properties_.previousScale = properties_.scale;
    properties_.scale = scale;
    objectMutexes_.scale.unlock();
    CalculateDerivedData();
}

void ZGameObject::SetOrientation(const glm::quat& quaternion)
{
    objectMutexes_.orientation.lock();
    properties_.previousOrientation = properties_.orientation;
    properties_.orientation = quaternion;
    objectMutexes_.orientation.unlock();

    std::shared_ptr<ZPhysicsComponent> physicsComp = FindComponent<ZPhysicsComponent>();
    if (physicsComp)
        physicsComp->RigidBody()->SetRotation(properties_.orientation);

    CalculateDerivedData();
}

void ZGameObject::SetOrientation(const glm::vec3& euler)
{
    objectMutexes_.orientation.lock();
    properties_.previousOrientation = properties_.orientation;
    properties_.orientation = glm::quat(euler);
    objectMutexes_.orientation.unlock();

    std::shared_ptr<ZPhysicsComponent> physicsComp = FindComponent<ZPhysicsComponent>();
    if (physicsComp)
        physicsComp->RigidBody()->SetRotation(properties_.orientation);

    CalculateDerivedData();
}

void ZGameObject::SetModelMatrix(const glm::mat4& modelMatrix)
{
    objectMutexes_.modelMatrix.lock();
    properties_.modelMatrix = modelMatrix;
    objectMutexes_.modelMatrix.unlock();
}

ZGameObjectMap ZGameObject::Load(std::shared_ptr<ZOFTree> data, const std::shared_ptr<ZScene>& scene)
{
    ZGameObjectMap gameObjects;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        std::shared_ptr<ZOFNode> node = it->second;
        std::shared_ptr<ZGameObject> gameObject;
        if (zenith::strings::HasObjectPrefix(node->id, "ZGO"))
        {
            gameObject = ZGameObject::Create(node, scene);
        }
        else if (zenith::strings::HasObjectPrefix(node->id, "ZLT"))
        {
            gameObject = ZLight::Create(node, scene);
        }
        else if (zenith::strings::HasObjectPrefix(node->id, "ZCAM"))
        {
            gameObject = ZCamera::Create(node, scene);
        }
        else if (zenith::strings::HasObjectPrefix(node->id, "ZSKY"))
        {
            gameObject = ZSkybox::Create(node, scene);
        }
        else if (zenith::strings::HasObjectPrefix(node->id, "ZGR"))
        {
            gameObject = ZGrass::Create(node, scene);
        }

        if (gameObject) {
            gameObjects[gameObject->ID()] = gameObject;

            for (ZOFChildMap::iterator compIt = node->children.begin(); compIt != node->children.end(); compIt++)
            {
                std::shared_ptr<ZOFNode> componentNode = compIt->second;
                ZComponent::CreateIn(compIt->first, gameObject, componentNode);
            }
        }
    }
    return gameObjects;
}

DEFINE_OBJECT_CREATORS(ZGameObject)