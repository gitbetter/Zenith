//
//  TMesh.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMaterial.hpp"
#include "ZMesh.hpp"
#include <string>

// Forward Declarations
class ZShader;
class ZCamera;

// Class Definitions
class ZMesh3D : public ZMesh {
  friend class ZModel;
public:
  std::vector<unsigned int> indices_;
  ZMaterial material_;

  ZMesh3D(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, ZMaterial material)
  : ZMesh(vertices) {
    indices_ = indices;
    material_ = material;
  }
  ~ZMesh3D() { }

  virtual void SetMaterial(ZMaterial material) { material_ = material; }

  virtual void Render(ZShader* shader) = 0;
};
