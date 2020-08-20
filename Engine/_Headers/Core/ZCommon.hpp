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

const unsigned int BonesPerVertex = 4;

enum ZKey
{
    UNKNOWN,
    SPACE,
    APOSTROPHE,
    COMMA,
    MINUS,
    PERIOD,
    SLASH,
    N0,
    N1,
    N2,
    N3,
    N4,
    N5,
    N6,
    N7,
    N8,
    N9,
    SEMICOLON,
    EQUAL,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LEFT_BRACKET,
    BACKSLASH,
    RIGHT_BRACKET,
    GRAVE_ACCENT,
    ESCAPE,
    ENTER,
    TAB,
    BACKSPACE,
    INSERT,
    DELETE,
    RIGHT,
    LEFT,
    DOWN,
    UP,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    CAPS_LOCK,
    SCROLL_LOCK,
    NUM_LOCK,
    PRINT_SCREEN,
    PAUSE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,
    KP_0,
    KP_1,
    KP_2,
    KP_3,
    KP_4,
    KP_5,
    KP_6,
    KP_7,
    KP_8,
    KP_9,
    KP_DECIMAL,
    KP_DIVIDE,
    KP_MULTIPLY,
    KP_SUBTRACT,
    KP_ADD,
    KP_ENTER,
    KP_EQUAL,
    LEFT_SHIFT,
    LEFT_CONTROL,
    LEFT_ALT,
    LEFT_SUPER,
    RIGHT_SHIFT,
    RIGHT_CONTROL,
    RIGHT_ALT,
    RIGHT_SUPER,
    MENU,
    ZKEY_LAST
};

enum ZMouse
{
    LEFT_MB,
    RIGHT_MB,
    MIDDLE_MB,
    MB4,
    MB5,
    MB6,
    MB7,
    MB8
};

class ZScene;
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

const std::string ENGINE_ASSETS_PATH(std::string(ENGINE_ROOT) + "/_Assets");

enum ZSeverity
{
    Info, Warning, Error
};

enum ZPriority
{
    FirstPriority, Critical = FirstPriority, High, Medium, Normal, Low, LastPriority
};

enum ZRenderPass
{
    First, Sky = First, Static, Dynamic, Invisible, UI, Last
};

enum ZRenderOp
{
    Depth, Shadow, Color, Post
};

enum class ZPlayState
{
    Playing, Paused, NotStarted
};

enum class ZPhysicsBodyType
{
    Dynamic = 1, Static = 2, Kinematic = 4, Trigger = 8, Character = 16, Particle = 32, All = -1
};

enum class ZCameraType
{
    Orthographic, Perspective
};

enum class ZCameraMovementStyle
{
    None, Normal, Follow
};

enum class ZLightType
{
    Directional, Point, Spot, Area, Hemisphere
};

enum class ZColliderType
{
    None, Box, Sphere, Capsule
};

enum class ZBufferDataType
{
    FrameBuffer, VertexArray
};

enum class ZCubemapTextureType
{
    Normal, Irradiance, Prefilter
};

enum class ZMeshDrawStyle
{
    Point, Line, LineStrip, Triangle, TriangleStrip, TriangleFan, Quads
};

enum class ZPrimitiveType
{
    Plane, Cube, Sphere, Cylinder, Cone
};

enum class ZUIElementType
{
    Button, CheckBox, Image, ListPanel, Panel, Text, Unknown
};

enum class ZShaderType
{
    Vertex, Pixel, Tesselation, Geometry, Other
};

enum class ZProcessState
{
    Uninitialized, Removed, Running, Paused, Finished, Failed, Aborted
};

enum class ZSoundType
{
    Wav, Ogg, Unknown
};

enum class ZResourceType
{
    ZOF, Model, VertexShader, PixelShader, TesselationShader,
    GeometryShader, Texture, HDRTexture, HDREquirectangularMap,
    Sound, Script, Font, Other
};

enum class ZAnimationState
{
    Initialized, Playing, Looping, Paused, Stopped, Invalid
};

struct ZBufferData
{
    ZBufferDataType type;
    union
    {
        struct
        {
            unsigned int vbo, ebo, vao;
        };
        struct
        {
            unsigned int fbo, rbo;
        };
    };
};

struct ZTexture
{
    unsigned int id;
    std::string type;
    std::string path;

    ZTexture() : id(0) {}
};

struct ZIBLTexture
{
    ZTexture cubeMap;
    ZTexture irradiance;
    ZTexture prefiltered;
    ZTexture brdfLUT;
};

struct ZMaterialProperties
{
    glm::vec4 albedo;
    float alpha;
    union
    {
        struct
        {
            float emission;
            float diffuse;
            float ambient;
            float specular;
            float shininess;
        };
        struct
        {
            float metallic;
            float roughness;
            float ao;
        };
    };

    ZMaterialProperties()
    {
        albedo = glm::vec4(1.f, 1.f, 1.f, 1.f);
        alpha = 1.f; emission = 0.f; diffuse = 0.f;
        ambient = 0.f; specular = 0.f; shininess = 0.f;
    }
};

struct ZVertex3D
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    unsigned int boneIDs[BonesPerVertex];
    float boneWeights[BonesPerVertex];

    ZVertex3D()
    {
        for (unsigned int i = 0; i < BonesPerVertex; i++) boneWeights[i] = 0.f;
    }

    ZVertex3D(glm::vec3 position, glm::vec3 normal = glm::vec3(0.f, 1.f, 0.f)) : ZVertex3D()
    {
        this->position = position;
        this->normal = normal;
    }

    void AddBoneData(unsigned int boneID, float weight)
    {
        for (unsigned int i = 0; i < BonesPerVertex; i++)
        {
            if (boneWeights[i] == 0.f)
            {
                boneIDs[i] = boneID;
                boneWeights[i] = weight;
                return;
            }
        }
    }
};

struct ZVertex2D
{
    glm::vec2 position;
    glm::vec2 uv;

    ZVertex2D() {}
    ZVertex2D(glm::vec2 position, glm::vec2 uv = glm::vec2(0.f)) : position(position), uv(uv) {}
};

struct ZCharacter
{
    ZTexture texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

struct ZFont
{
    std::string name;
    std::map<unsigned char, ZCharacter> characters;
};

struct ZOFLoadResult
{
    ZGameObjectMap gameObjects;
    ZUIElementMap uiElements;
};

struct ZWavFormatDesc
{
    unsigned short formatTag;
    unsigned short channels;
    unsigned long samplesPerSec;
    unsigned long avgBytesPerSec;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    unsigned short cbSize;
};

struct ZRaycastHitResult
{
    ZGameObject* objectHit;
    glm::vec3 hitPosition;
    bool hasHit;
};

struct ZEngineOptions
{
    bool drawPhysicsDebug = false;
    bool drawCameraDebug = false;
    bool drawGrid = false;
};

struct ZDomainOptions
{
    unsigned int width = 1920;
    unsigned int height = 1080;
    bool maximized = true;
    bool visible = true;
};

struct ZSceneSnapshot
{
    std::shared_ptr<ZScene> scene;
    double time;
};
