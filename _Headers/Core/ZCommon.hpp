//
//  ZCommon.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define ZINFO 0
#define ZWARNING 1
#define ZERROR 2

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <cassert>
#include <type_traits>

#define _Z(message, severity) { \
  switch(severity) {\
    case ZINFO:\
        std::cout << "\033[1;97m" << "[Info]: " << __FILE__ << ":" << __LINE__ << ": " << message << "\033[0m" << std::endl;\
        break;\
    case ZWARNING:\
        std::cout << "\033[1;33m" << "[Warning]: " << __FILE__ << ":" << __LINE__ << ": " << message << "\033[0m" << std::endl;\
        break;\
    case ZERROR:\
        std::cout << "\033[1;91m" << "[Error]: " << __FILE__ << ":" << __LINE__ << ": " << message << "\033[0m" << std::endl;\
        break;\
  }\
}

enum ZEventType {
  FirePress
};

typedef std::function<void()> ZEventCallback;

enum ZCameraType {
  Orthographic, Perspective
};

enum ZCameraMovementStyle {
  None, Normal, Follow
};

enum ZLightType {
  Directional, Point, Spot, Area, Hemisphere
};

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

struct ZVertex3D {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 tangent;
  glm::vec3 bitangent;

  ZVertex3D() { }
  ZVertex3D(glm::vec3 position, glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f)) : position(position), normal(normal) { }
};

struct ZVertex2D {
  glm::vec2 position;
  glm::vec2 uv;

  ZVertex2D() { }
  ZVertex2D(glm::vec2 position, glm::vec2 uv = glm::vec2(0.f)) : position(position), uv(uv) { }
};

struct ZCharacter {
  ZTexture texture;
  glm::ivec2 size;
  glm::ivec2 bearing;
  unsigned int advance;
};

struct ZFont {
  std::string name;
  std::map<unsigned char, ZCharacter> characters;
};

enum ZPrimitiveType {
  Plane, Cube, Sphere, Cylinder, Cone
};

enum ZShaderTypes {
  Vertex, Pixel, Tesselation, Geometry, Other
};
