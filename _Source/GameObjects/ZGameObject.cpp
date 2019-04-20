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
#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZModel.hpp"
#include "ZCameraComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZOFTree.hpp"
#include "ZIDSequence.hpp"

ZGameObject::ZGameObject(glm::vec3 position, glm::quat orientation) {
    properties_.position = glm::vec4(position, 1.f);
    properties_.scale = glm::vec3(1.f, 1.f, 1.f);
    properties_.orientation = orientation;
    properties_.modelMatrix = glm::mat4(1.f);
    properties_.renderPass = ZRenderPass::First;
    id_ = "ZGO_" + ZEngine::IDSequence()->Next();
    CalculateDerivedData();
}

void ZGameObject::Initialize(std::shared_ptr<ZOFNode> root) {
    ZProcess::Initialize();
    
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if(node == nullptr) {
        _Z("Could not initalize ZGameObject", ZERROR);
        return;
    }
    
    id_ = node->id;
    
    ZOFPropertyMap props = node->properties;
    
    if (props.find("position") != props.end() && props["position"]->HasValues()) {
        std::shared_ptr<ZOFNumberList> posProp = props["position"]->Value<ZOFNumberList>(0);
        properties_.position = glm::vec4(posProp->value[0], posProp->value[1], posProp->value[2], 1.f);
        properties_.previousPosition = properties_.position;
    }
    
    if (props.find("orientation") != props.end() && props["orientation"]->HasValues()) {
        std::shared_ptr<ZOFNumberList> ornProp = props["orientation"]->Value<ZOFNumberList>(0);
        properties_.orientation = glm::quat(glm::vec3(ornProp->value[0], ornProp->value[1], ornProp->value[2]));
        properties_.previousOrientation = properties_.orientation;
    }
    
    if (props.find("scale") != props.end() && props["scale"]->HasValues()) {
        std::shared_ptr<ZOFNumberList> scaleProp = props["scale"]->Value<ZOFNumberList>(0);
        properties_.scale = glm::vec3(scaleProp->value[0], scaleProp->value[1], scaleProp->value[2]);
        properties_.previousScale = properties_.scale;
    }
    
    CalculateDerivedData();
}

void ZGameObject::Render(float frameMix, RENDER_OP renderOp) {
    std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>();
    if (graphicsComp != nullptr) {
        std::shared_ptr<ZGameObject> camera = scene_->ActiveCamera();
        const ZLightMap& gameLights = scene_->GameLights();
        graphicsComp->SetGameLights(gameLights);
        graphicsComp->SetGameCamera(camera);
        graphicsComp->Render(frameMix, renderOp);
    }
}

void ZGameObject::RenderChildren(float frameMix, RENDER_OP renderOp) {
    ZGameObjectList::iterator it = children_.begin(), end = children_.end();
    for (; it != end; it++) {
        (*it)->PreRender();
        if ((*it)->IsVisible()) {
            (*it)->Render(frameMix, renderOp);
        }
        (*it)->RenderChildren(frameMix, renderOp);
        (*it)->PostRender();
    }
}

void ZGameObject::CalculateDerivedData() {
    properties_.orientation = glm::normalize(properties_.orientation);
    
    properties_.modelMatrix = glm::mat4_cast(properties_.orientation);
    properties_.modelMatrix = glm::scale(properties_.modelMatrix, properties_.scale);
    properties_.modelMatrix = glm::translate(properties_.modelMatrix, glm::vec3(properties_.position));
    
    std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>();
    if (graphicsComp) graphicsComp->Model()->UpdateAABB(properties_.modelMatrix);
}

void ZGameObject::AddChild(std::shared_ptr<ZGameObject> gameObject) {
    if (std::find(children_.begin(), children_.end(), gameObject) == children_.end()) {
        if (!gameObject->scene_) gameObject->scene_ = scene_;
        children_.push_back(gameObject);
    }
}

void ZGameObject::RemoveChild(std::shared_ptr<ZGameObject> gameObject) {
    ZGameObjectList::iterator it = std::find(children_.begin(), children_.end(), gameObject);
    if (it != children_.end())
    children_.erase(it);
}

bool ZGameObject::IsVisible() {
    std::shared_ptr<ZGameObject> camera = scene_->ActiveCamera();
    std::shared_ptr<ZCameraComponent> cameraComp = camera->FindComponent<ZCameraComponent>();
    std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>();
    if (cameraComp && graphicsComp) {
        return cameraComp->Frustum().Contains(graphicsComp->Model()->AABB());
    }
    return false;
}

void ZGameObject::SetPosition(glm::vec3 position) {
    // TODO: set the btRigidBody transform position in the
    // physics component, if there is one
    properties_.previousPosition = properties_.position;
    properties_.position = glm::vec4(position, 1.f);
    CalculateDerivedData();
}

void ZGameObject::SetScale(glm::vec3 scale) {
    // TODO: set the btRigidBody local scaling in the
    // physics component, if there is one
    properties_.previousScale = properties_.scale;
    properties_.scale = scale;
    CalculateDerivedData();
}

void ZGameObject::SetOrientation(glm::quat quaternion) {
    // TODO: set the btRigidBody transform orientation in the
    // physics component, if there is one
    properties_.previousOrientation = properties_.orientation;
    properties_.orientation = quaternion;
    CalculateDerivedData();
}

void ZGameObject::SetOrientation(glm::vec3 euler) {
    // TODO: set the btRigidBody transform orientation in the
    // physics component, if there is one
    properties_.previousOrientation = properties_.orientation;
    properties_.orientation = glm::quat(euler);
    CalculateDerivedData();
}

void ZGameObject::SetModelMatrix(glm::mat4 modelMatrix) {
    properties_.modelMatrix = modelMatrix;
}
