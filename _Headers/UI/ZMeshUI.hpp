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
#include <glm/glm.hpp>

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZMeshUI : ZMesh {
private:
public:
  ZMeshUI(std::vector<glm::vec4> vertices) : vertices_(vertices) { }
  ~ZMeshUI() { }

  void Render(ZShader* shader) override;

protected:
  std::vector<glm::vec4> vertices_;
};
