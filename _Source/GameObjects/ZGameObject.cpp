//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGameObject.hpp"
#include "ZCommon.hpp"
#include "ZIDSequence.hpp"
#include "ZCameraComponent.hpp"
#include "ZOFTree.hpp"

ZGameObject::ZGameObject(glm::vec3 position, glm::quat orientation)
: position_(glm::vec4(position, 1.f)),
  orientation_(orientation),
  scale_(glm::vec3(1.f, 1.f, 1.f)),
  modelMatrix_(glm::mat4(1.f)),
  translatesWithView_(false) {
  id_ = "ZGO_" + ZEngine::IDSequence()->Next();
  CalculateModelMatrix();
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

  CalculateModelMatrix();
}

void ZGameObject::Update() {
  ZCameraComponent* cameraComp = FindComponent<ZCameraComponent>();
  if (cameraComp != nullptr) {
    cameraComp->UpdateCameraOrientation();
  }
}

void ZGameObject::ShouldTranslateWithView(bool translates) {
  translatesWithView_ = translates;
  ZGraphicsComponent* graphicsComp = FindComponent<ZGraphicsComponent>();
  if (graphicsComp != nullptr) {
    graphicsComp->ShouldTranslateWithView(translatesWithView_);
  }
}

void ZGameObject::SetPosition(glm::vec3 position) {
  previousPosition_ = position_;
  position_ = glm::vec4(position, 1.f);
  CalculateModelMatrix();
}

void ZGameObject::SetScale(glm::vec3 scale) {
  previousScale_ = scale_;
  scale_ = scale;
  CalculateModelMatrix();
}

void ZGameObject::SetOrientation(glm::quat quaternion) {
  previousOrientation_ = orientation_;
  orientation_ = quaternion;
  CalculateModelMatrix();
}

void ZGameObject::SetOrientation(glm::vec3 euler) {
  previousOrientation_ = orientation_;
  orientation_ = glm::quat(euler);
  CalculateModelMatrix();
}

void ZGameObject::CalculateModelMatrix() {
  modelMatrix_ = glm::mat4_cast(orientation_);
  modelMatrix_ = glm::scale(modelMatrix_, scale_);
  modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(position_));
}
