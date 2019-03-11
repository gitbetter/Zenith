//
//  ZUIButton.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUIButton.hpp"
#include "ZShader.hpp"
#include "ZEngine.hpp"
#include "ZCommon.hpp"
#include "ZUI.hpp"

ZUIButton::ZUIButton(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
   texture_ = ZEngine::Graphics()->Strategy()->LoadDefaultTexture();
   enabled_ = true;
}

void ZUIButton::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
}

void ZUIButton::Draw(ZShader* shader) {
  ZUIElement::Draw(shader);
  ElementShape().Render(shader);
  RenderChildren(shader);
}

ZMeshUI ZUIButton::ElementShape() {
  static ZMeshUI mesh;
  if (mesh.Vertices().size() == 0) {
    std::vector<ZVertex2D> vertices = {
     ZVertex2D(glm::vec2(-1.f, 1.f)),
     ZVertex2D(glm::vec2(-1.f, -1.f)),
     ZVertex2D(glm::vec2(1.f, 1.f)),
     ZVertex2D(glm::vec2(1.f, -1.f))
    };
    mesh = ZMeshUI(vertices);
  }
  return mesh;
}
