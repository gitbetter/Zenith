//
//  ZCommon.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define ZINFO 0
#define ZWARNING 1
#define ZERROR 2

#include "ZOFTree.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <cassert>
#include <type_traits>
#include <memory>

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

#define RENDER_OP_COLOR 0x01
#define RENDER_OP_SHADOW 0x02

class ZGameObject;
class ZShader;
class ZMesh3D;
class ZMaterial;

typedef std::function<void()> ZEventCallback;
typedef std::map<std::string, std::shared_ptr<ZGameObject>> ZGameObjectMap;
typedef std::map<std::string, std::shared_ptr<ZShader>> ZShaderMap;
typedef std::map<std::string, std::shared_ptr<ZMesh3D>> ZMesh3DMap;
typedef std::map<std::string, std::shared_ptr<ZMaterial>> ZMaterialMap;
typedef unsigned char RENDER_OP;

enum class ZEventType {
  FirePress
};

enum class ZCameraType {
  Orthographic, Perspective
};

enum class ZCameraMovementStyle {
  None, Normal, Follow
};

enum class ZLightType {
  Directional, Point, Spot, Area, Hemisphere
};

enum class ZColliderType {
  None, Box, Sphere, Capsule
};

enum class ZBufferDataType {
  FrameBuffer, VertexArray
};

enum class ZCubemapTextureType {
  Normal, Irradiance, Prefilter
};

enum class ZMeshDrawStyle {
  Point, Line, LineStrip, Triangle, TriangleStrip
};

struct ZBufferData {
  ZBufferDataType type;
  union {
    struct {
      unsigned int vbo, ebo, vao;
    };
    struct {
      unsigned int fbo, rbo;
    };
  };
};

struct ZTexture {
  unsigned int id;
  std::string type;
  std::string path;
};

struct ZIBLTexture {
  ZTexture cubeMap;
  ZTexture irradiance;
  ZTexture prefiltered;
  ZTexture brdfLUT;
};

struct ZMaterialProperties {
  glm::vec4 albedo;
  union {
    struct {
      float emission;
      float diffuse;
      float ambient;
      float specular;
      float shininess;
    };
    struct {
      float metallic;
      float roughness;
      float ao;
    };
  };
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

enum class ZPrimitiveType {
  Plane, Cube, Sphere, Cylinder, Cone
};

enum class ZShaderTypes {
  Vertex, Pixel, Tesselation, Geometry, Other
};
