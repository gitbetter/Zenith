//
//  ZMesh3D.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZMesh3D.hpp"
#include "ZEngine.hpp"
#include "ZShader.hpp"
#include "ZCommon.hpp"

ZMesh3D::ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMaterial material, ZMeshDrawStyle drawStyle)
: vertices_(vertices), indices_(indices), material_(material) {
  drawStyle_ = drawStyle;
  bufferData_ = ZEngine::Graphics()->Strategy()->LoadIndexedVertexData(vertices, indices);
}

ZMesh3D::ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMaterial material, ZVertex3D min, ZVertex3D max, ZMeshDrawStyle drawStyle) : ZMesh3D(vertices, indices, material, drawStyle) {
  minVertex_ = min; maxVertex_ = max;
}

void ZMesh3D::Render(ZShader* shader) {
  shader->Activate();
  shader->Use(material_);
  ZEngine::Graphics()->Strategy()->Draw(bufferData_, vertices_, indices_, drawStyle_);
}
