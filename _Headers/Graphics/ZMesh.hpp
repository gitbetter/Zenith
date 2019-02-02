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
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

// Forward Declarations
class ZShader;
class ZCamera;

// Class Definitions
struct ZVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 tangent;
  glm::vec3 bitangent;
};

class ZMesh {
  friend class ZModel;
private:
  unsigned int vao_, vbo_, ebo_;
  void Setup();
public:
  std::vector<ZVertex> vertices_;
  std::vector<unsigned int> indices_;
  ZMaterial material_;

  ZMesh(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, ZMaterial material);
  virtual ~ZMesh() { }

  virtual void Render(ZShader* shader);
};
