//
//  ZMesh3D.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh.hpp"
#include "ZMaterial.hpp"
#include <string>

// Forward Declarations
class ZShader;

// Class Definitions
class ZMesh3D : public ZMesh {
  friend class ZModel;
private:

  std::vector<ZVertex3D> vertices_;
  std::vector<unsigned int> indices_;
  ZMaterial material_;
  // Used for calculating bounds of collision primitives and BVH nodes
  ZVertex3D minVertex_, maxVertex_;

public:

  ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMaterial material);
  ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMaterial material, ZVertex3D min, ZVertex3D max);
  ~ZMesh3D() { }

  ZVertex3D Min() const { return minVertex_; }
  ZVertex3D Max() const { return maxVertex_; }

  void SetMaterial(ZMaterial material) { material_ = material; }

  void Render(ZShader* shader);
};
