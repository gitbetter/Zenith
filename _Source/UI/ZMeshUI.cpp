//
//  ZMeshUI.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZMeshUI.hpp"
#include "ZEngine.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZShader.hpp"

ZMeshUI::ZMeshUI(std::vector<glm::vec4> vertices) : vertices_(vertices) {
  bufferData_ = ZEngine::Graphics()->Strategy()->LoadVertexData(vertices);
}

void ZMeshUI::Render(ZShader* shader) {
  shader->Activate();
  ZEngine::Graphics()->Strategy()->Draw(bufferData_, vertices_);
}
