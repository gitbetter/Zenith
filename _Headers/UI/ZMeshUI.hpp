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
  ZMeshUI(std::vector<glm::vec4> vertices);
  ~ZMeshUI() { }

  void Render(ZShader* shader) override;

  std::vector<glm::vec4> Vertices() const { return vertices_; }

protected:
  std::vector<glm::vec4> vertices_;
};
