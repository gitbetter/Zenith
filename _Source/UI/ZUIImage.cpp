//
//  ZUIImage.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZUI.hpp"
#include "ZUIImage.hpp"
#include "ZShader.hpp"
#include "ZEngine.hpp"
#include "ZLogger.hpp"

ZUIImage::ZUIImage(std::string path, glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
  texture_ = ZEngine::UI()->Strategy()->LoadTexture(path, "");
  texture_.type = "image";
}

void ZUIImage::Render(ZShader* shader) {
  ZUIElement::Render(shader);
  ZEngine::UI()->Strategy()->EnableAlphaBlending();
  ElementShape().Render(shader);
  ZEngine::UI()->Strategy()->DisableAlphaBlending();
}

ZMeshUI ZUIImage::ElementShape() {
  static ZMeshUI mesh;
  if (mesh.Vertices().size() == 0) {
    std::vector<float> vertices = {
      -1.f, 1.f,
      -1.f, -1.f,
      1.f, 1.f,
      1.f, -1.f
    };
    mesh = ZMeshUI(vertices);
  }
  return mesh;
}
