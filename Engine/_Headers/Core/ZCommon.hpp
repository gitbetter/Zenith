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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <initializer_list>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <set>
#include <functional>
#include <cassert>
#include <type_traits>
#include <memory>
#include <regex>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include "ZOFTree.hpp"
#include "FastDelegate.h"

#define _Z(message, severity) { \
if (zenith::Logger()) zenith::Log(message, severity);\
}

#define BONES_PER_VERTEX 4

#define ZKEY_UNKNOWN            -1
#define ZKEY_SPACE              32
#define ZKEY_APOSTROPHE         39
#define ZKEY_COMMA              44
#define ZKEY_MINUS              45
#define ZKEY_PERIOD             46
#define ZKEY_SLASH              47
#define ZKEY_0                  48
#define ZKEY_1                  49
#define ZKEY_2                  50
#define ZKEY_3                  51
#define ZKEY_4                  52
#define ZKEY_5                  53
#define ZKEY_6                  54
#define ZKEY_7                  55
#define ZKEY_8                  56
#define ZKEY_9                  57
#define ZKEY_SEMICOLON          59
#define ZKEY_EQUAL              61
#define ZKEY_A                  65
#define ZKEY_B                  66
#define ZKEY_C                  67
#define ZKEY_D                  68
#define ZKEY_E                  69
#define ZKEY_F                  70
#define ZKEY_G                  71
#define ZKEY_H                  72
#define ZKEY_I                  73
#define ZKEY_J                  74
#define ZKEY_K                  75
#define ZKEY_L                  76
#define ZKEY_M                  77
#define ZKEY_N                  78
#define ZKEY_O                  79
#define ZKEY_P                  80
#define ZKEY_Q                  81
#define ZKEY_R                  82
#define ZKEY_S                  83
#define ZKEY_T                  84
#define ZKEY_U                  85
#define ZKEY_V                  86
#define ZKEY_W                  87
#define ZKEY_X                  88
#define ZKEY_Y                  89
#define ZKEY_Z                  90
#define ZKEY_LEFT_BRACKET       91
#define ZKEY_BACKSLASH          92
#define ZKEY_RIGHT_BRACKET      93
#define ZKEY_GRAVE_ACCENT       96
#define ZKEY_ESCAPE             256
#define ZKEY_ENTER              257
#define ZKEY_TAB                258
#define ZKEY_BACKSPACE          259
#define ZKEY_INSERT             260
#define ZKEY_DELETE             261
#define ZKEY_RIGHT              262
#define ZKEY_LEFT               263
#define ZKEY_DOWN               264
#define ZKEY_UP                 265
#define ZKEY_PAGE_UP            266
#define ZKEY_PAGE_DOWN          267
#define ZKEY_HOME               268
#define ZKEY_END                269
#define ZKEY_CAPS_LOCK          280
#define ZKEY_SCROLL_LOCK        281
#define ZKEY_NUM_LOCK           282
#define ZKEY_PRINT_SCREEN       283
#define ZKEY_PAUSE              284
#define ZKEY_F1                 290
#define ZKEY_F2                 291
#define ZKEY_F3                 292
#define ZKEY_F4                 293
#define ZKEY_F5                 294
#define ZKEY_F6                 295
#define ZKEY_F7                 296
#define ZKEY_F8                 297
#define ZKEY_F9                 298
#define ZKEY_F10                299
#define ZKEY_F11                300
#define ZKEY_F12                301
#define ZKEY_F13                302
#define ZKEY_F14                303
#define ZKEY_F15                304
#define ZKEY_F16                305
#define ZKEY_F17                306
#define ZKEY_F18                307
#define ZKEY_F19                308
#define ZKEY_F20                309
#define ZKEY_F21                310
#define ZKEY_F22                311
#define ZKEY_F23                312
#define ZKEY_F24                313
#define ZKEY_F25                314
#define ZKEY_KP_0               320
#define ZKEY_KP_1               321
#define ZKEY_KP_2               322
#define ZKEY_KP_3               323
#define ZKEY_KP_4               324
#define ZKEY_KP_5               325
#define ZKEY_KP_6               326
#define ZKEY_KP_7               327
#define ZKEY_KP_8               328
#define ZKEY_KP_9               329
#define ZKEY_KP_DECIMAL         330
#define ZKEY_KP_DIVIDE          331
#define ZKEY_KP_MULTIPLY        332
#define ZKEY_KP_SUBTRACT        333
#define ZKEY_KP_ADD             334
#define ZKEY_KP_ENTER           335
#define ZKEY_KP_EQUAL           336
#define ZKEY_LEFT_SHIFT         340
#define ZKEY_LEFT_CONTROL       341
#define ZKEY_LEFT_ALT           342
#define ZKEY_LEFT_SUPER         343
#define ZKEY_RIGHT_SHIFT        344
#define ZKEY_RIGHT_CONTROL      345
#define ZKEY_RIGHT_ALT          346
#define ZKEY_RIGHT_SUPER        347
#define ZKEY_MENU               348
#define ZKEY_LAST               ZKEY_MENU

#define ZMOUSE_LEFT      0
#define ZMOUSE_RIGHT     1
#define ZMOUSE_MIDDLE    2
#define ZMOUSE_4         3
#define ZMOUSE_5         4
#define ZMOUSE_6         5
#define ZMOUSE_7         6
#define ZMOUSE_8         7
#define ZMOUSE_LAST      ZMOUSE_8

class ZGameObject;
class ZComponent;
class ZUIElement;
class ZModel;
class ZShader;
class ZMesh3D;
class ZMaterial;
class ZProcess;
class ZEvent;
class ZLight;
struct ZAnimation;
struct ZBone;

typedef std::function<void()> ZEventCallback;
typedef std::map<std::string, std::shared_ptr<ZGameObject>> ZGameObjectMap;
typedef std::map<std::string, std::shared_ptr<ZLight>> ZLightMap;
typedef std::map<std::string, std::shared_ptr<ZUIElement>> ZUIElementMap;
typedef std::map<std::string, std::shared_ptr<ZShader>> ZShaderMap;
typedef std::map<std::string, std::shared_ptr<ZModel>> ZModelMap;
typedef std::map<std::string, std::shared_ptr<ZMesh3D>> ZMesh3DMap;
typedef std::map<std::string, std::shared_ptr<ZMaterial>> ZMaterialMap;
typedef std::vector<std::shared_ptr<ZGameObject>> ZGameObjectList;
typedef std::vector<std::shared_ptr<ZComponent>> ZComponentList;
typedef std::list<std::shared_ptr<ZProcess>> ZProcessList;
typedef fastdelegate::FastDelegate1<std::shared_ptr<ZEvent>> ZEventDelegate;
typedef std::pair<ZGameObject*, ZGameObject*> ZCollisionPair;
typedef std::set<ZCollisionPair> ZCollisionPairs;
typedef std::map<std::string, std::shared_ptr<ZAnimation>> ZAnimationMap;
typedef std::map<std::string, unsigned int> ZBoneMap;
typedef std::vector<std::shared_ptr<ZBone>> ZBoneList;

typedef unsigned long ZEventType;

enum ZPriority {
	FirstPriority, Critical = FirstPriority, High, Medium, Normal, Low, LastPriority
};

enum ZRenderPass {
    First, Sky = First, Static, Dynamic, Invisible, UI, Last
};

enum ZRenderOp {
	Depth, Shadow, Color, Post
};

enum class ZPlayState {
    Playing, Paused, NotStarted
};

enum class ZPhysicsBodyType {
    Dynamic = 1, Static = 2, Kinematic = 4, Trigger = 8, Character = 16, Particle = 32, All = -1
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

enum class ZPrimitiveType {
    Plane, Cube, Sphere, Cylinder, Cone
};

enum class ZShaderType {
    Vertex, Pixel, Tesselation, Geometry, Other
};

enum class ZProcessState {
    Uninitialized, Removed, Running, Paused, Finished, Failed, Aborted
};

enum class ZSoundType {
    Wav, Ogg, Unknown
};

enum class ZResourceType {
	ZOF, Model, VertexShader, PixelShader, TesselationShader, 
	GeometryShader, Texture, HDRTexture, HDREquirectangularMap,
	Sound, Script, Font, Other
};

enum class ZAnimationState {
	Initialized, Playing, Looping, Paused, Stopped, Invalid
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
    float alpha;
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

	ZMaterialProperties() {
		albedo = glm::vec4(1.f, 1.f, 1.f, 1.f);
		alpha = 1.f; emission = 0.f; diffuse = 0.f;
		ambient = 0.f; specular = 0.f; shininess = 0.f;
	}
};

struct ZVertex3D {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
	unsigned int boneIDs[BONES_PER_VERTEX];
	float boneWeights[BONES_PER_VERTEX];
    
    ZVertex3D() {
        for (unsigned int i = 0; i < BONES_PER_VERTEX; i++) boneWeights[i] = 0.f;
    }
    
    ZVertex3D(glm::vec3 position, glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f)) : ZVertex3D() {
        this->position = position;
        this->normal = normal;
    }

    void AddBoneData(unsigned int boneID, float weight) {
        for (unsigned int i = 0; i < BONES_PER_VERTEX; i++) {
            if (boneWeights[i] == 0.f) {
                boneIDs[i] = boneID;
                boneWeights[i] = weight;
                return;
            }
        }
    }
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

struct ZWavFormatDesc {
    unsigned short formatTag;
    unsigned short channels;
    unsigned long samplesPerSec;
    unsigned long avgBytesPerSec;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    unsigned short cbSize;
};

struct ZRaycastHitResult {
    ZGameObject* objectHit;
    glm::vec3 hitPosition;
    bool hasHit;
};

struct ZEngineOptions {
	bool drawPhysicsDebug = false;
	bool drawCameraDebug = false;
};
