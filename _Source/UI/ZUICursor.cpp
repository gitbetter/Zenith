//
//  ZUICursor.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZUICursor.hpp"
#include "ZUIImage.hpp"
#include "ZDomain.hpp"
#include "ZUI.hpp"
#include "ZCommon.hpp"

ZUICursor::ZUICursor(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
  ZUIImage* cursorImage = new ZUIImage("Assets/Textures/z_cursor.png", position, scale);
  AddChild(cursorImage);
}

void ZUICursor::Render(ZShader* shader) {
  ZUIElement::Render(shader);
  RenderChildren(shader);
}

void ZUICursor::SetCursorImage(std::string path) {
  for (ZUIElement* child : children_) {
    ZUIImage* uiImage = dynamic_cast<ZUIImage*>(child);
    if (uiImage != nullptr) {
      uiImage->SetImage(path);
    }
  }
}

void ZUICursor::SetColor(glm::vec4 color) {
  ZUIElement::SetColor(color);
  for (ZUIElement* child : children_) {
    ZUIImage* uiImage = dynamic_cast<ZUIImage*>(child);
    if (uiImage != nullptr) {
      uiImage->SetColor(color);
    }
  }
}

void ZUICursor::HandlePitch(float controlThrow) {
  Translate(glm::vec2(0.f, -controlThrow * 0.05f * ZEngine::DeltaTime()));
}

void ZUICursor::HandleYaw(float controlThrow) {
  Translate(glm::vec2(controlThrow * 0.05f * ZEngine::DeltaTime(), 0.f));
}

void ZUICursor::HandleFire() {
  std::vector<ZUIElement*> elements = ZEngine::UI()->Elements();
  for (ZUIElement* element : elements) {
    if (!element->Selectable()) continue;
    if (Position().x >= element->Position().x - element->Size().x && Position().x <= element->Position().x + element->Size().x &&
        Position().y >= element->Position().y - element->Size().y && Position().y <= element->Position().y + element->Size().y) {
          element->Fire(ZEventType::FirePress);
    }
  }
}
