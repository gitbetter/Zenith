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
  std::shared_ptr<ZMaterial> material_;

public:

  ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, std::shared_ptr<ZMaterial> material, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle);
  ~ZMesh3D() { }

  void SetMaterial(std::shared_ptr<ZMaterial> material) { material_ = material; }

  void Render(ZShader* shader);
};
