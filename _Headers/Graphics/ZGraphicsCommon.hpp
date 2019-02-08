//
//  ZGraphicsCommon.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 07/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

struct ZBufferData {
  unsigned int vbo, ebo, vao;
};

struct ZTexture {
  unsigned int id;
  std::string type;
  std::string path;
};

struct ZMaterialProperties {
  glm::vec4 albedo;
  glm::vec3 emission;
  glm::vec3 diffuse;
  glm::vec3 ambient;
  glm::vec3 specular;
  float shininess;
};

struct ZVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 tangent;
  glm::vec3 bitangent;

  ZVertex() { }
  ZVertex(glm::vec3 position, glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f)) : position(position), normal(normal) { }
};

enum ZPrimitiveType {
  Plane, Cube, Sphere, Cylinder, Cone
};

enum ZShaderTypes {
  Vertex, Pixel, Tesselation, Geometry, Other
};
