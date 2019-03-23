/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZCommon.hpp

    Created by Adrian Sanchez on 11/02/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define ZINFO 0
#define ZWARNING 1
#define ZERROR 2

#include "ZOFTree.hpp"
#include "FastDelegate.h"
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
#include <list>
#include <functional>
#include <cassert>
#include <type_traits>
#include <memory>
#include <regex>
#include <algorithm>
#include <sstream>
#include <chrono>

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

class ZGameObject;
class ZLight;
class ZUIElement;
class ZShader;
class ZMesh3D;
class ZMaterial;
class ZEvent;

typedef std::function<void()> ZEventCallback;
typedef std::map<std::string, std::shared_ptr<ZGameObject>> ZGameObjectMap;
typedef std::map<std::string, std::shared_ptr<ZLight>> ZLightMap;
typedef std::map<std::string, std::shared_ptr<ZUIElement>> ZUIElementMap;
typedef std::map<std::string, std::shared_ptr<ZShader>> ZShaderMap;
typedef std::map<std::string, std::shared_ptr<ZMesh3D>> ZMesh3DMap;
typedef std::map<std::string, std::shared_ptr<ZMaterial>> ZMaterialMap;
typedef fastdelegate::FastDelegate1<std::shared_ptr<ZEvent>> ZEventDelegate;

typedef unsigned char RENDER_OP;
typedef unsigned long ZEventType;

#define RENDER_OP_COLOR 0x01
#define RENDER_OP_SHADOW 0x02

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

enum class ZPrimitiveType {
  Plane, Cube, Sphere, Cylinder, Cone
};

enum class ZShaderTypes {
  Vertex, Pixel, Tesselation, Geometry, Other
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

struct ZOFLoadResult {
  ZGameObjectMap gameObjects;
  ZUIElementMap uiElements;
};
