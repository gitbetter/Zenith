//
//  ZMeshUI.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZUI.hpp"
#include "ZMeshUI.hpp"
#include "ZEngine.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZShader.hpp"

ZMeshUI::ZMeshUI(std::vector<ZVertex2D> vertices) : vertices_(vertices) {
  bufferData_ = ZEngine::UI()->GraphicsStrategy()->LoadVertexData(vertices);
}

void ZMeshUI::Render(ZShader* shader) {
  shader->Activate();
  ZEngine::UI()->GraphicsStrategy()->Draw(bufferData_, vertices_);
}
