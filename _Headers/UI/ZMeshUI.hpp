//
//  ZMeshUI.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh.hpp"
#include "ZCommon.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZMeshUI : ZMesh {
private:
public:
  ZMeshUI() { }
  ZMeshUI(std::vector<ZVertex2D> vertices);
  ~ZMeshUI() { }

  void Render(ZShader* shader) override;

  std::vector<ZVertex2D> Vertices() const { return vertices_; }

protected:
  std::vector<ZVertex2D> vertices_;
};
