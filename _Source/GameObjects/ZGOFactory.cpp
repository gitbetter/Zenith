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
#include "ZGraphicsComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZCameraComponent.hpp"

ZGOFactory::ZGOFactory() {
  componentCreators_["GraphicsComponent"] = &ZGOFactory::CreateGraphicsComponent;
  componentCreators_["CameraComponent"] = &ZGOFactory::CreateCameraComponent;
  componentCreators_["PhysicsComponent"] = &ZGOFactory::CreatePhysicsComponent;
}

ZGameObjectMap ZGOFactory::Create(ZOFTree* data) {
  ZGameObjectMap gameObjects;
  for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
    ZOFNode* node = it->second;
    if (node->id.find("ZGO") == 0) {
      std::shared_ptr<ZGameObject> gameObject(new ZGameObject);
      gameObject->Initialize(node);

      for (ZOFChildMap::iterator compIt = node->children.begin(); compIt != node->children.end(); compIt++) {
        ZOFNode* componentNode = compIt->second;
        if (componentCreators_.find(compIt->first) != componentCreators_.end()) {
          std::shared_ptr<ZComponent> comp = (this->*componentCreators_[compIt->first])(gameObject);
          comp->Initialize(componentNode);
        } else {
          _Z("Component " + compIt->first + " is not available for creation", ZWARNING);
        }
      }

      gameObjects[gameObject->ID()] = gameObject;
    }
  }
  return gameObjects;
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
