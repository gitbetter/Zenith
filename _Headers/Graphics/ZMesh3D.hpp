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
class ZCamera;

// Class Definitions
class ZMesh3D : public ZMesh {
  friend class ZModel;
private:
  std::vector<ZVertex3D> vertices_;
  std::vector<unsigned int> indices_;
  ZMaterial material_;

  void AttachMaterialTextures(ZShader* shader);

public:
  ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMaterial material);
  ~ZMesh3D() { }

  void SetMaterial(ZMaterial material) { material_ = material; }

  void Render(ZShader* shader);
};
