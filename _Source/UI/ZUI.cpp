//
//  ZUI.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
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
  if(textStrategy_ == nullptr) {
    textStrategy_.reset(new ZGLTextStrategy);
    textStrategy_->Initialize();
  }

  if (uiShader_ == nullptr) {
    uiShader_ = std::shared_ptr<ZShader>(new ZShader);
    uiShader_->Initialize("Assets/Shaders/Vertex/ui.vert", "Assets/Shaders/Pixel/ui.frag");
  }

  if (textShader_ == nullptr) {
    textShader_ = std::shared_ptr<ZShader>(new ZShader);
    textShader_->Initialize("Assets/Shaders/Vertex/text.vert", "Assets/Shaders/Pixel/text.frag");
  }
}

void ZUI::Draw() {
  if (cursor_ != nullptr) cursor_->Draw(uiShader_.get());
  for (std::shared_ptr<ZUIElement> element : elements_) {
      element->Draw((std::dynamic_pointer_cast<ZUIText>(element)) ? textShader_.get() : uiShader_.get());
  }
}

void ZUI::AddElement(std::shared_ptr<ZUIElement> element) {
  if (element != nullptr) {
    // TODO: Check if this ui element is already in the array before the push_back
    elements_.push_back(element);
  }
}

void ZUI::AddElements(std::initializer_list<std::shared_ptr<ZUIElement>> elements) {
  for (std::shared_ptr<ZUIElement> element : elements) {
    AddElement(element);
  }
}

void ZUI::EnableCursor() {
  if (cursor_ == nullptr) {
    cursor_ = std::shared_ptr<ZUICursor>(new ZUICursor);
    cursor_->SetColor(glm::vec4(1.f));
    ZEngine::Input()->Register(cursor_);
  }
}

void ZUI::DisableCursor() {
  if (cursor_ != nullptr) {
    cursor_ = nullptr;
  }
}

void ZUI::RegisterFont(std::string fontPath) {
  if (textStrategy_ != nullptr) textStrategy_->LoadFont(fontPath, 64);
}

void ZUI::CleanUp() {
  for (std::shared_ptr<ZUIElement> element : elements_) {
    element->CleanUp();
  }
  elements_.clear();

  if (cursor_ != nullptr) {
    cursor_->CleanUp();
    cursor_ = nullptr;
  }

  if (uiShader_ != nullptr) {
    uiShader_ = nullptr;
  }
}
