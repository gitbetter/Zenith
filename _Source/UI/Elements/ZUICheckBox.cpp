//
//  ZUICheckBox.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUICheckBox.hpp"
#include "ZEngine.hpp"
#include "ZEventAgent.hpp"
#include "ZShader.hpp"
#include "ZEngine.hpp"
#include "ZCommon.hpp"
#include "ZUIImage.hpp"
#include "ZUI.hpp"
#include "ZObjectSelectedEvent.hpp"

ZUICheckBox::ZUICheckBox(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
   texture_ = ZEngine::Graphics()->Strategy()->LoadDefaultTexture();
}

void ZUICheckBox::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
    
  checkImage_ = std::make_shared<ZUIImage>("Assets/Textures/UI/checkmark.png", glm::vec3(0.f) + Size(), Size());
  AddChild(checkImage_);

  ZEventDelegate fireDelegate = fastdelegate::MakeDelegate(this, &ZUICheckBox::HandleMousePress);
  ZEngine::EventAgent()->AddListener(fireDelegate, ZObjectSelectedEvent::Type);

  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZUICursor", ZERROR); return;
  }

  ZOFPropertyMap props = node->properties;

  if (props.find("checkColor") != props.end() && props["checkColor"]->HasValues()) {
    ZOFNumberList* chkColorProp = props["checkColor"]->Value<ZOFNumberList>(0);
    checkImage_->SetColor(glm::vec4(chkColorProp->value[0], chkColorProp->value[1], chkColorProp->value[2], chkColorProp->value[3]));
  }
}

void ZUICheckBox::Draw(ZShader* shader) {
  ZMeshUI mesh = ElementShape();
  Render(shader, &mesh);
  RenderChildren(shader);
}

ZMeshUI ZUICheckBox::ElementShape() {
  return ZUIElement::ElementShape();
}

void ZUICheckBox::HandleMousePress(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZObjectSelectedEvent> fireEvent = std::dynamic_pointer_cast<ZObjectSelectedEvent>(event);
  if (id_ == fireEvent->ObjectID()) {
    checked_ = !checked_;
    if (checked_) {
      checkImage_->Show();
    } else {
      checkImage_->Hide();
    }
  }
}
