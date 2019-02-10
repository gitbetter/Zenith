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
#include "ZLogger.hpp"

ZUICursor::ZUICursor(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
  ZUIImage* cursorImage = new ZUIImage("Assets/Textures/z_cursor.png", position, scale);
  AddChild(cursorImage);
}

void ZUICursor::Render(ZShader* shader) {
  ZUIElement::Render(shader);
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
  // TODO: Mouse movement is choppy and imprecise. Might have to do with the
  // almost concurrent behavior of the HandlePitch and HandleYaw observer methods
  Translate(glm::vec2(0.f, -controlThrow * 0.1f * ZEngine::DeltaTime()));
}

void ZUICursor::HandleYaw(float controlThrow) {
  // TODO: Mouse movement is choppy and imprecise. Might have to do with the
  // almost concurrent behavior of the HandlePitch and HandleYaw observer methods
  Translate(glm::vec2(controlThrow * 0.1f * ZEngine::DeltaTime(), 0.f));
}

void ZUICursor::HandleFire() {

}
