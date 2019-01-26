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
  vector<ZVertex> vertices_;
  vector<unsigned int> indices_;
  vector<ZTexture> textures_;

  ZMesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ZTexture> textures);
  virtual void Render();
};
