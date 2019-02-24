//
//  ZCollisionComponent.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 23/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGameObject.hpp"
#include "ZModel.hpp"
#include "ZCollisionComponent.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZPhysicsComponent.hpp"

void ZCollisionComponent::Initialize(ZOFNode* root) {
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZCollisionComponent", ZERROR);
    return;
  }

  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() == 0) continue;

    if (prop->id == "primitive" && collisionPrimitive_ == nullptr) {
      glm::vec3 min(-1.f), max(1.f);
      ZGraphicsComponent* graphicsComp = object_->FindComponent<ZGraphicsComponent>();
      if (graphicsComp && graphicsComp->Model()) {
        min = graphicsComp->Model()->MinVertex().position;
        max = graphicsComp->Model()->MaxVertex().position;
      }

      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      collisionPrimitive_ = ZCollisionPrimitive::Create(terminal->value, min, max);
      collisionPrimitive_->body = object_;

      ZPhysicsComponent* physicsComp = object_->FindComponent<ZPhysicsComponent>();
      if (physicsComp) physicsComp->SetInertiaTensor(collisionPrimitive_);
    }
  }
}

void ZCollisionComponent::Update() {
  if (collisionPrimitive_ != nullptr)
    collisionPrimitive_->CalculateInternals();
}
