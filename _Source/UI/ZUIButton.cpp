//
//  ZUIButton.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZUIButton.hpp"

ZMeshUI ZUIButton::ElementShape() {
  static ZMeshUI mesh;
  if (mesh.Vertices().size() == 0) {
    std::vector<glm::vec4> vertices = {
      glm::vec4(glm::vec2(-1.f), glm::vec2(0.f)),
      glm::vec4(glm::vec2(-1.f, 1.f), glm::vec2(0.f)),
      glm::vec4(glm::vec2(1.f), glm::vec2(0.f)),
      glm::vec4(glm::vec2(1.f, -1.f), glm::vec2(0.f))
    };
    mesh = ZMeshUI(vertices);
  }
  return mesh;
}
