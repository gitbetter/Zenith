//
//  ZUIPanel.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUIPanel.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"

void ZUIPanel::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
  texture_ = ZEngine::Graphics()->Strategy()->LoadDefaultTexture();
}

void ZUIPanel::Draw(ZShader* shader) {
  ZUIElement::Draw(shader);
  ElementShape().Render(shader);
  RenderChildren(shader);
}

ZMeshUI ZUIPanel::ElementShape() {
  return ZUIElement::ElementShape();
}