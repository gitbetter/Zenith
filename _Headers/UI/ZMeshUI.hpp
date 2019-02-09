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

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZMeshUI : ZMesh {
private:
public:
  ZMeshUI() { }
  ZMeshUI(std::vector<float> vertices);
  ~ZMeshUI() { }

  void Render(ZShader* shader) override;

  std::vector<float> Vertices() const { return vertices_; }

protected:
  std::vector<float> vertices_;
};
