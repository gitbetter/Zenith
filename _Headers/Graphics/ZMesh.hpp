//
//  ZMesh.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZLogger.hpp"
#include "ZMaterial.hpp"
#include <glm/glm.hpp>
#include <vector>

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
struct ZVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 tangent;
  glm::vec3 bitangent;

  ZVertex() { }
  ZVertex(glm::vec3 position, glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f)) : position(position), normal(normal) { }
};

class ZMesh {
private:
public:
  ZMesh(std::vector<ZVertex> vertices) : vertices_(vertices) { }
  virtual ~ZMesh() { }

  virtual void Render(ZShader* shader) = 0;

  std::vector<ZVertex> vertices_;
protected:

};
