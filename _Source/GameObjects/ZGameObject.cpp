//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGameObject.hpp"
#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZCameraComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZOFTree.hpp"
#include "ZIDSequence.hpp"

ZGameObject::ZGameObject(glm::vec3 position, glm::quat orientation)
: position_(glm::vec4(position, 1.f)),
  scale_(glm::vec3(1.f, 1.f, 1.f)),
  orientation_(orientation),
  modelMatrix_(glm::mat4(1.f)) {
  id_ = "ZGO_" + ZEngine::IDSequence()->Next();
  CalculateDerivedData();
}

void ZGameObject::Initialize(ZOFNode* root) {
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZGameObject", ZERROR);
    return;
  }

  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() > 0) {
      ZOFNumberList* terminal = dynamic_cast<ZOFNumberList*>(prop->values[0]);
      if (prop->id == "position") {
        position_ = glm::vec4(terminal->value[0], terminal->value[1], terminal->value[2], 1.f);
      } else if (prop->id == "orientation") {
        orientation_ = glm::quat(glm::vec3(terminal->value[0], terminal->value[1], terminal->value[2]));
      } else if (prop->id == "scale") {
        scale_ = glm::vec3(terminal->value[0], terminal->value[1], terminal->value[2]);
      }
    }
  }

  CalculateDerivedData();
}

void ZGameObject::Update() {
  ZPhysicsComponent* physicsComp = FindComponent<ZPhysicsComponent>();
  if (physicsComp) physicsComp->Update();

  ZCameraComponent* cameraComp = FindComponent<ZCameraComponent>();
  if (cameraComp) cameraComp->Update();
}

void ZGameObject::Render(float frameMix, unsigned char renderOp) {
  ZGraphicsComponent* graphicsComp = FindComponent<ZGraphicsComponent>();
  if (graphicsComp != nullptr) {
    ZGameObject* camera = game_->ActiveCamera();
    const std::map<std::string, ZLight*>& gameLights = game_->GetGameLights();
    graphicsComp->SetGameLights(gameLights);
    graphicsComp->SetGameCamera(camera);
    graphicsComp->Render(frameMix, renderOp);
  }
}

void ZGameObject::SetPosition(glm::vec3 position) {
  // TODO: set the btRigidBody transform position in the
  // physics component, if there is one
  previousPosition_ = position_;
  position_ = glm::vec4(position, 1.f);
  CalculateDerivedData();
}

void ZGameObject::SetScale(glm::vec3 scale) {
  // TODO: set the btRigidBody local scaling in the
  // physics component, if there is one
  previousScale_ = scale_;
  scale_ = scale;
  CalculateDerivedData();
}

void ZGameObject::SetOrientation(glm::quat quaternion) {
  // TODO: set the btRigidBody transform orientation in the
  // physics component, if there is one
  previousOrientation_ = orientation_;
  orientation_ = quaternion;
  CalculateDerivedData();
}

void ZGameObject::SetOrientation(glm::vec3 euler) {
  // TODO: set the btRigidBody transform orientation in the
  // physics component, if there is one
  previousOrientation_ = orientation_;
  orientation_ = glm::quat(euler);
  CalculateDerivedData();
}

void ZGameObject::SetModelMatrix(glm::mat4 modelMatrix) {
  modelMatrix_ = modelMatrix;
}

void ZGameObject::CalculateDerivedData() {
  orientation_ = glm::normalize(orientation_);

  modelMatrix_ = glm::mat4_cast(orientation_);
  modelMatrix_ = glm::scale(modelMatrix_, scale_);
  modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(position_));
}
