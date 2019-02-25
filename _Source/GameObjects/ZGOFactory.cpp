//
//  ZGOFactory.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 18/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

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
      ZGameObject* gameObject = new ZGameObject;
      gameObject->Initialize(node);

      for (ZOFChildMap::iterator compIt = node->children.begin(); compIt != node->children.end(); compIt++) {
        ZOFNode* componentNode = compIt->second;
        if (componentCreators_.find(compIt->first) != componentCreators_.end()) {
          ZComponent* comp = (this->*componentCreators_[compIt->first])(gameObject);
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

ZComponent* ZGOFactory::CreateGraphicsComponent(ZGameObject* gameObject) {
  ZGraphicsComponent* comp = new ZGraphicsComponent;
  gameObject->AddComponent(comp);
  return comp;
}

ZComponent* ZGOFactory::CreateCameraComponent(ZGameObject* gameObject) {
  ZCameraComponent* comp = new ZCameraComponent;
  gameObject->AddComponent(comp);
  return comp;
}

ZComponent* ZGOFactory::CreatePhysicsComponent(ZGameObject* gameObject) {
  ZPhysicsComponent* comp = new ZPhysicsComponent;
  gameObject->AddComponent(comp);
  return comp;
}
