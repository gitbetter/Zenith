/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUICheckBox.cpp

    Created by Adrian Sanchez on 16/03/2019.
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
   texture_ = zenith::Graphics()->Strategy()->LoadDefaultTexture();
}

void ZUICheckBox::Initialize(std::shared_ptr<ZOFNode> root) {
  ZUIElement::Initialize(root);
    
  checkImage_ = std::make_shared<ZUIImage>("Assets/Textures/UI/checkmark.png", glm::vec3(0.f) + Size(), Size());
  AddChild(checkImage_);

  ZEventDelegate fireDelegate = fastdelegate::MakeDelegate(this, &ZUICheckBox::HandleMousePress);
  zenith::EventAgent()->AddListener(fireDelegate, ZObjectSelectedEvent::Type);

  std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZUICursor", ZERROR); return;
  }

  ZOFPropertyMap props = node->properties;

  if (props.find("checkColor") != props.end() && props["checkColor"]->HasValues()) {
    std::shared_ptr<ZOFNumberList> chkColorProp = props["checkColor"]->Value<ZOFNumberList>(0);
    checkImage_->SetColor(glm::vec4(chkColorProp->value[0], chkColorProp->value[1], chkColorProp->value[2], chkColorProp->value[3]));
  }
}

void ZUICheckBox::Render(ZRenderOp renderOp) {
  ZUIElement::Render();
  RenderChildren();
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