//
//  TMesh.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

// Forward Declarations
class ZShader;

// Class Definitions
struct ZVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
};

struct ZTexture {
  unsigned int id;
  std::string type;
};

class ZMesh {
  friend class ZModel;
private:
  unsigned int arrayObj_, bufferObj_, elementBufferObj_;
  void Setup();
public:
  std::vector<ZVertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<ZTexture> textures_;

  ZMesh(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, std::vector<ZTexture> textures);
  virtual void Render(ZShader* shader);
};
