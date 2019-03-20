//
//  ZUIListPanel.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUIListPanel.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"

void ZUIListPanel::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
  texture_ = ZEngine::Graphics()->Strategy()->LoadDefaultTexture();

   ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZUIElement", ZERROR);
    return;
  }

  ZOFPropertyMap props = node->properties;

  if (props.find("itemHeight") != props.end() && props["itemHeight"]->HasValues()) {
    ZOFNumber* itemHeightProp = props["itemHeight"]->Value<ZOFNumber>(0);
    itemHeight_ = itemHeightProp->value;
  }
}

void ZUIListPanel::Draw(ZShader* shader) {
  ZMeshUI mesh = ElementShape();
  Render(shader, &mesh);
  RenderChildren(shader);
}

ZMeshUI ZUIListPanel::ElementShape() {
  return ZUIElement::ElementShape();
}

void ZUIListPanel::AddChild(std::shared_ptr<ZUIElement> element) {
  element->SetPosition(glm::vec2(Size().x, 2.f * itemHeight_ * children_.size() + itemHeight_));
  element->SetSize(glm::vec2(Size().x, itemHeight_));

  SetSize(glm::vec2(Size().x, glm::max(Size().y, itemHeight_ * children_.size())));

  ZUIElement::AddChild(element);  
} 