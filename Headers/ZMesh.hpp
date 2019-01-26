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
    virtual void Render();
};
