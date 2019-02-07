//
//  ZGLMesh3D.hpp
//  geek-text
//
//  Created by Adrian Sanchez on 03/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh3D.hpp"

// Forward Declarations
// class SomeClass;

// TODO: Create enums for OGL layout attributes and magic numbers
// Class and Data Structure Definitions
class ZGLMesh3D : public ZMesh3D {
private:
  unsigned int vao_, vbo_, ebo_;

  void Setup();

public:
  ZGLMesh3D(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, ZMaterial material)
  : ZMesh3D(vertices, indices, material) { Setup(); }
  ~ZGLMesh3D() { }

  void Render(ZShader* shader) override;
};
