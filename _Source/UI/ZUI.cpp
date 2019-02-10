//
//  ZUI.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

// TODO: Conditional include based on graphics implementation
#include "ZGLGraphicsStrategy.hpp"
#include "ZUI.hpp"
#include "ZShader.hpp"
#include "ZUICursor.hpp"
#include "ZEngine.hpp"
#include "ZInput.hpp"

void ZUI::Initialize() {
  // TODO: Switch the strategy here based on graphics implementation
  if (graphicsStrategy_ == nullptr) {
    graphicsStrategy_ = new ZGLGraphicsStrategy();
    graphicsStrategy_->Initialize();
  }

  if (uiShader_ == nullptr) {
    uiShader_ = new ZShader("Assets/Shaders/Vertex/ui.vert", "Assets/Shaders/Pixel/ui.frag");
  }
}

void ZUI::Draw() {
  if (cursor_ != nullptr) cursor_->Render(uiShader_);
  for (ZUIElement* element : elements_) {
    element->Render(uiShader_);
  }
}

void ZUI::AddElement(ZUIElement* element) {
  // TODO: Check if UI element exists before adding
  elements_.push_back(element);
}

void ZUI::AddElements(std::initializer_list<ZUIElement*> elements) {
  for (ZUIElement* element : elements) {
    AddElement(element);
  }
}

void ZUI::EnableCursor() {
  if (cursor_ == nullptr) {
    cursor_ = new ZUICursor();
    cursor_->SetColor(glm::vec4(1.f));
    ZEngine::Input()->Register(cursor_);
  }
}

void ZUI::DisableCursor() {
  if (cursor_ != nullptr) {
    delete cursor_;
    cursor_ = nullptr;
  }
}

void ZUI::CleanUp() {
  for (ZUIElement* element : elements_) {
    element->CleanUp();
    delete element;
  }

  if (cursor_ != nullptr) {
    cursor_->CleanUp();
    delete cursor_;
  }

  if (graphicsStrategy_ != nullptr) {
    delete graphicsStrategy_;
  }

  if (uiShader_ != nullptr) {
    delete uiShader_;
  }
}
