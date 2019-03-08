//
//  ZMesh3D.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZMesh3D.hpp"
#include "ZEngine.hpp"
#include "ZShader.hpp"
#include "ZCommon.hpp"

ZMesh3D::ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMeshDrawStyle drawStyle)
: vertices_(vertices), indices_(indices) {
  drawStyle_ = drawStyle;
  bufferData_ = ZEngine::Graphics()->Strategy()->LoadIndexedVertexData(vertices, indices);
  id_ = "ZMSH3D_" + ZEngine::IDSequence()->Next();
}

void ZMesh3D::Render(ZShader* shader, ZMaterial* material) {
  shader->Activate();
  shader->Use(material);
  ZEngine::Graphics()->Strategy()->Draw(bufferData_, vertices_, indices_, drawStyle_);
}
