/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
   ZGOFactory.cpp
 
   Created by Adrian Sanchez on 18/02/2019.
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

#include "ZGOFactory.hpp"
#include "ZGameObject.hpp"
#include "ZLight.hpp"
#include "ZTrigger.hpp"
#include "ZSkybox.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZCameraComponent.hpp"
#include "ZAnimatorComponent.hpp"

ZGOFactory::ZGOFactory() {
    componentCreators_["GraphicsComponent"] = &ZGOFactory::CreateGraphicsComponent;
    componentCreators_["CameraComponent"] = &ZGOFactory::CreateCameraComponent;
    componentCreators_["PhysicsComponent"] = &ZGOFactory::CreatePhysicsComponent;
    componentCreators_["AnimatorComponent"] = &ZGOFactory::CreateAnimatorComponent;
}

ZGameObjectMap ZGOFactory::Load(std::shared_ptr<ZOFTree> data) {
    ZGameObjectMap gameObjects;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
        std::shared_ptr<ZOFNode> node = it->second;
        if (node->id.find("ZGO") == 0) {
            std::shared_ptr<ZGameObject> gameObject = CreateGameObject(node);
            gameObjects[gameObject->ID()] = gameObject;
        } else if (node->id.find("ZLT") == 0) {
            std::shared_ptr<ZLight> light = CreateLight(node);
            gameObjects[light->ID()] = light;
        } else if (node->id.find("ZTR") == 0) {
            std::shared_ptr<ZTrigger> trigger = CreateTrigger(node);
            gameObjects[trigger->ID()] = trigger;
        } else if (node->id.find("ZSKY") == 0) {
            std::shared_ptr<ZSkybox> skybox = CreateSkybox(node);
            gameObjects[skybox->ID()] = skybox;
        }
    }
    return gameObjects;
}

std::shared_ptr<ZGameObject> ZGOFactory::CreateGameObject(std::shared_ptr<ZOFNode> data) {
    std::shared_ptr<ZGameObject> gameObject(new ZGameObject);
    gameObject->Initialize(data);
    
    for (ZOFChildMap::iterator compIt = data->children.begin(); compIt != data->children.end(); compIt++) {
        std::shared_ptr<ZOFNode> componentNode = compIt->second;
        if (componentCreators_.find(compIt->first) != componentCreators_.end()) {
            std::shared_ptr<ZComponent> comp = (this->*componentCreators_[compIt->first])(gameObject);
            comp->Initialize(componentNode);
        } else {
            zenith::Log("Component " + compIt->first + " is not available for creation", ZSeverity::Warning);
        }
    }
    
    return gameObject;
}

std::shared_ptr<ZLight> ZGOFactory::CreateLight(std::shared_ptr<ZOFNode> data) {
    std::shared_ptr<ZLight> light(new ZLight);
    light->Initialize(data);
    return light;
}

std::shared_ptr<ZTrigger> ZGOFactory::CreateTrigger(std::shared_ptr<ZOFNode> data) {
    std::shared_ptr<ZTrigger> trigger(new ZTrigger);
    trigger->Initialize(data);
    return trigger;
}

std::shared_ptr<ZSkybox> ZGOFactory::CreateSkybox(std::shared_ptr<ZOFNode> data) {
    std::shared_ptr<ZSkybox> skybox(new ZSkybox);
    skybox->Initialize(data);
    return skybox;
}

std::shared_ptr<ZComponent> ZGOFactory::CreateGraphicsComponent(std::shared_ptr<ZGameObject> gameObject) {
    std::shared_ptr<ZGraphicsComponent> comp(new ZGraphicsComponent);
	gameObject->AddComponent(comp);
    return comp;
}

std::shared_ptr<ZComponent> ZGOFactory::CreateCameraComponent(std::shared_ptr<ZGameObject> gameObject) {
    std::shared_ptr<ZCameraComponent> comp(new ZCameraComponent);
	gameObject->AddComponent(comp);
    return comp;
}

std::shared_ptr<ZComponent> ZGOFactory::CreatePhysicsComponent(std::shared_ptr<ZGameObject> gameObject) {
    std::shared_ptr<ZPhysicsComponent> comp(new ZPhysicsComponent);
	gameObject->AddComponent(comp);
    return comp;
}

std::shared_ptr<ZComponent> ZGOFactory::CreateAnimatorComponent(std::shared_ptr<ZGameObject> gameObject) {
    std::shared_ptr<ZAnimatorComponent> comp(new ZAnimatorComponent);
	gameObject->AddComponent(comp);
    return comp;
}
