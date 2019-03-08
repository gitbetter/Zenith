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
}

void ZUIImage::Draw(ZShader* shader) {
  ZUIElement::Draw(shader);
  ZEngine::Graphics()->Strategy()->EnableAlphaBlending();
  ElementShape().Render(shader);
  ZEngine::Graphics()->Strategy()->DisableAlphaBlending();
  RenderChildren(shader);
}

ZMeshUI ZUIImage::ElementShape() {
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

void ZUIImage::SetImage(std::string path) {
  if (!path.empty()) {
    texture_ = ZEngine::Graphics()->Strategy()->LoadTexture(path, "");
    texture_.type = "image";
  }
}
