//
//  ZUIButton.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZUIButton.hpp"
#include "ZShader.hpp"
#include "ZEngine.hpp"
#include "ZLogger.hpp"

void ZUIButton::Render(ZShader* shader) {
  ZUIElement::Render(shader);
  ElementShape().Render(shader);
}

ZMeshUI ZUIButton::ElementShape() {
  static ZMeshUI mesh;
  if (mesh.Vertices().size() == 0) {
    std::vector<float> vertices = {
      -1.f, 1.f, 0.f, 0.f,
      -1.f, -1.f, 0.f, 0.f,
      1.f, 1.f, 0.f, 0.f,
      1.f, -1.f, 0.f, 0.f
    };
    mesh = ZMeshUI(vertices);
  }
  return mesh;
}
