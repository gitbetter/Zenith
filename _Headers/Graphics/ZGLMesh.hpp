//
//  ZGLMesh.hpp
//  geek-text
//
//  Created by Adrian Sanchez on 03/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh.hpp"
#include <vector>

// Forward Declarations
// class SomeClass;

// TODO: Create enums for OGL layout attributes and magic numbers
// Class and Data Structure Definitions
class ZGLMesh : public ZMesh {
private:
  unsigned int vao_, vbo_, ebo_;

  void Setup();

public:
  ZGLMesh(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, ZMaterial material)
  : ZMesh(vertices, indices, material) { Setup(); }
  ~ZGLMesh() { }

  void Render(ZShader* shader) override;
};
