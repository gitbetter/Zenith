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
