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
#include "ZFireEvent.hpp"

ZUICheckBox::ZUICheckBox(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
   texture_ = ZEngine::Graphics()->Strategy()->LoadDefaultTexture();
   enabled_ = true;
}

void ZUICheckBox::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
    
  checkImage_ = std::make_shared<ZUIImage>("Assets/Textures/UI/checkmark.png", glm::vec3(0.f) + Size(), Size());
  checkImage_->SetColor(glm::vec4(0.8f, 0.3f, 0.1f, 1.f));
  AddChild(checkImage_);

  ZEventDelegate fireDelegate = fastdelegate::MakeDelegate(this, &ZUICheckBox::HandleMousePress);
  ZEngine::EventAgent()->AddListener(fireDelegate, ZFireEvent::Type);
}

void ZUICheckBox::Draw(ZShader* shader) {
  ZUIElement::Draw(shader);
  ElementShape().Render(shader);
  ZUIElement::RenderChildren(shader);
}

ZMeshUI ZUICheckBox::ElementShape() {
  return ZUIElement::ElementShape();
}

void ZUICheckBox::HandleMousePress(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZFireEvent> fireEvent = std::dynamic_pointer_cast<ZFireEvent>(event);

  checked_ = !checked_;
  if (checked_) {
    checkImage_->Show();
  } else {
    checkImage_->Hide();
  }
}
