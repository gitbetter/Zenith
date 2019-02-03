//
//  TMesh.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZLogger.hpp"
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

  ZVertex() { }
  ZVertex(glm::vec3 position, glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f)) : position(position), normal(normal) { }
};

class ZMesh {
  friend class ZModel;
public:
  std::vector<ZVertex> vertices_;
  std::vector<unsigned int> indices_;
  ZMaterial material_;

  ZMesh(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, ZMaterial material)
  : vertices_(vertices), indices_(indices), material_(material) { }
  virtual ~ZMesh() { }

  virtual void SetMaterial(ZMaterial material) { material_ = material; }

  virtual void Render(ZShader* shader) = 0;
};
