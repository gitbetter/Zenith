//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
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

  id_ = node->id;

  ZOFPropertyMap props = node->properties;

  if (props.find("position") != props.end() && props["position"]->HasValues()) {
    ZOFNumberList* posProp = props["position"]->Value<ZOFNumberList>(0);
    position_ = glm::vec4(posProp->value[0], posProp->value[1], posProp->value[2], 1.f);
  }

  if (props.find("orientation") != props.end() && props["orientation"]->HasValues()) {
    ZOFNumberList* ornProp = props["orientation"]->Value<ZOFNumberList>(0);
    orientation_ = glm::quat(glm::vec3(ornProp->value[0], ornProp->value[1], ornProp->value[2]));
  }

  if (props.find("scale") != props.end() && props["scale"]->HasValues()) {
    ZOFNumberList* scaleProp = props["scale"]->Value<ZOFNumberList>(0);
    scale_ = glm::vec3(scaleProp->value[0], scaleProp->value[1], scaleProp->value[2]);
  }

  CalculateDerivedData();
}

void ZGameObject::Update() {
  std::shared_ptr<ZPhysicsComponent> physicsComp = FindComponent<ZPhysicsComponent>();
  if (physicsComp) physicsComp->Update();

  std::shared_ptr<ZCameraComponent> cameraComp = FindComponent<ZCameraComponent>();
  if (cameraComp) cameraComp->Update();
}

void ZGameObject::Render(float frameMix, unsigned char renderOp) {
  std::shared_ptr<ZGraphicsComponent> graphicsComp = FindComponent<ZGraphicsComponent>();
  if (graphicsComp != nullptr) {
    std::shared_ptr<ZGameObject> camera = game_->ActiveCamera();
    const std::map<std::string, std::shared_ptr<ZLight>>& gameLights = game_->GameLights();
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
