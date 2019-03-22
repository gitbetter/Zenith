/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUICursor.cpp

    Created by Adrian Sanchez on 09/02/2019.
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

#include "ZCommon.hpp"
#include "ZEngine.hpp"
#include "ZEventAgent.hpp"
#include "ZObjectLookEvent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZFireEvent.hpp"
#include "ZRaycastEvent.hpp"
#include "ZUICursor.hpp"
#include "ZUIImage.hpp"
#include "ZDomain.hpp"
#include "ZUI.hpp"
#include "ZGame.hpp"
#include "ZPhysics.hpp"
#include "ZCameraComponent.hpp"

void ZUICursor::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
  
  auto cursorImage = std::make_shared<ZUIImage>("Assets/Textures/UI/z_cursor.png", Position(), Size());
  AddChild(cursorImage);
  cursorImage->SetColor(Color());

  ZEventDelegate lookDelegate = fastdelegate::MakeDelegate(this, &ZUICursor::HandleMouseMove);
  ZEventDelegate fireDelegate = fastdelegate::MakeDelegate(this, &ZUICursor::HandleMousePress);
  ZEngine::EventAgent()->AddListener(lookDelegate, ZObjectLookEvent::Type);
  ZEngine::EventAgent()->AddListener(fireDelegate, ZFireEvent::Type);

  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZUICursor", ZERROR); return;
  }

  ZOFPropertyMap props = node->properties;

  if (props.find("speed") != props.end() && props["speed"]->HasValues()) {
    ZOFNumber* speedProp = props["speed"]->Value<ZOFNumber>(0);
    cursorSpeed_ = speedProp->value;
  }
}

void ZUICursor::Render(float frameMix, RENDER_OP renderOp) {
  ZUIElement::Render();
  RenderChildren();
}

void ZUICursor::SetCursorImage(std::string path) {
  for (std::shared_ptr<ZUIElement> child : children_) {
    std::shared_ptr<ZUIImage> uiImage = std::dynamic_pointer_cast<ZUIImage>(child);
    if (uiImage != nullptr) {
      uiImage->SetImage(path);
      uiImage->SetColor(Color());
    }
  }
}

void ZUICursor::SetColor(glm::vec4 color) {
  ZUIElement::SetColor(color);
  for (std::shared_ptr<ZUIElement> child : children_) {
    std::shared_ptr<ZUIImage> uiImage = std::dynamic_pointer_cast<ZUIImage>(child);
    if (uiImage != nullptr) {
      uiImage->SetColor(color);
    }
  }
}

void ZUICursor::HandleMouseMove(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZObjectLookEvent> lookEvent = std::static_pointer_cast<ZObjectLookEvent>(event);
  Translate(glm::vec2(0.f, -lookEvent->Pitch() * cursorSpeed_ * ZEngine::DeltaTime()));
  Translate(glm::vec2(lookEvent->Yaw() * cursorSpeed_ * ZEngine::DeltaTime(), 0.f));
}

void ZUICursor::HandleMousePress(std::shared_ptr<ZEvent> event) {
  // Create a ZUISelectedEvent to handle UI element selection
  ZUIElementMap elements = ZEngine::UI()->Elements();
  bool uiSelected = false;
  for (ZUIElementMap::iterator it = elements.begin(); it != elements.end(); it++) {
    uiSelected = it->second->TrySelect(Position());
  }

  // If a UI element is selected, underlying game objects should not be picked (i.e. Z ordering is preserved)
  if (uiSelected) return;

  // Create a ZRaycastEvent to handle ray casting if no UI elements were selected
  std::shared_ptr<ZRaycastEvent> raycastEvent(new ZRaycastEvent(glm::vec3(Position().x / (float)ZEngine::Domain()->ResolutionX(), 
                                                                          Position().y / (float)ZEngine::Domain()->ResolutionY(), 
                                                                          0.f)));
  ZEngine::EventAgent()->QueueEvent(raycastEvent);
}
