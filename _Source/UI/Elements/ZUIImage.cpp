//
//  ZUIImage.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUI.hpp"
#include "ZUIImage.hpp"
#include "ZShader.hpp"
#include "ZEngine.hpp"
#include "ZCommon.hpp"

ZUIImage::ZUIImage(std::string path, glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
  SetImage(path);
  enabled_ = false;
}

void ZUIImage::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
}

void ZUIImage::Draw(ZShader* shader) {
  ZEngine::Graphics()->Strategy()->EnableAlphaBlending();
  ZMeshUI mesh = ElementShape();
  Render(shader, &mesh);
  ZEngine::Graphics()->Strategy()->DisableAlphaBlending();
  RenderChildren(shader);
}

ZMeshUI ZUIImage::ElementShape() {
  return ZUIElement::ElementShape();
}

void ZUIImage::SetImage(std::string path) {
  if (!path.empty()) {
    texture_ = ZEngine::Graphics()->Strategy()->LoadTexture(path, "");
    texture_.type = "image";
  }
}
