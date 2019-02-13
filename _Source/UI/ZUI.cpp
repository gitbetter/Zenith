//
//  ZUI.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZUI.hpp"
#include "ZShader.hpp"
#include "ZUICursor.hpp"
#include "ZEngine.hpp"
#include "ZInput.hpp"
#include "ZUIText.hpp"
#include "ZGLTextStrategy.hpp"

void ZUI::Initialize() {
  // TODO: Switch the strategies here based on implementation details
  if (graphicsStrategy_ == nullptr) {
    graphicsStrategy_ = new ZGLGraphicsStrategy();
    graphicsStrategy_->Initialize();
  }

  if (uiShader_ == nullptr) {
    uiShader_ = new ZShader("Assets/Shaders/Vertex/ui.vert", "Assets/Shaders/Pixel/ui.frag");
  }

  if(textStrategy_ == nullptr) {
    textStrategy_ = new ZGLTextStrategy();
    textStrategy_->Initialize();
  }

  if (textShader_ == nullptr) {
    textShader_ = new ZShader("Assets/Shaders/Vertex/text.vert", "Assets/Shaders/Pixel/text.frag");
  }
}

void ZUI::Draw() {
  if (cursor_ != nullptr) cursor_->Render(uiShader_);
  for (ZUIElement* element : elements_) {
      element->Render((dynamic_cast<ZUIText*>(element)) ? textShader_ : uiShader_);
  }
}

void ZUI::AddElement(ZUIElement* element) {
  if (element != nullptr) {
    // TODO: Check if this ui element is already in the array before the push_back
    elements_.push_back(element);
  }
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

void ZUI::RegisterFont(std::string fontPath) {
  if (textStrategy_ != nullptr) textStrategy_->LoadFont(fontPath, 64);
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
