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
#include <unordered_map>
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
#include <filesystem>
#include "ZOFTree.hpp"
#include "ZLogger.hpp"
#include "ZIDSequence.hpp"
#include "ZStringHelpers.hpp"

constexpr unsigned int BONES_PER_VERTEX = 4;
constexpr float DEFAULT_X_RESOLUTION = 2560.f;
constexpr float DEFAULT_Y_RESOLUTION = 1600.f;
constexpr float UPDATE_STEP_SIZE = 0.017f;
constexpr int MAX_FIXED_UPDATE_ITERATIONS = 24;
constexpr unsigned int SHADOW_MAP_SIZE = 4096;
constexpr unsigned int CUBE_MAP_SIZE = 2048;
constexpr unsigned int IRRADIANCE_MAP_SIZE = 32;
constexpr unsigned int PREFILTER_MAP_SIZE = 128;
constexpr unsigned int LUT_SIZE = 512;
const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);
const glm::vec3 WORLD_RIGHT(1.0f, 0.0f, 0.0f);
const glm::vec3 WORLD_FRONT(0.0f, 0.0f, -1.0f);
const std::vector<std::string> DEFAULT_SKYBOX_CUBEMAP{
    "/Skyboxes/Default/right.png",
    "/Skyboxes/Default/left.png",
    "/Skyboxes/Default/up.png",
    "/Skyboxes/Default/down.png",
    "/Skyboxes/Default/front.png",
    "/Skyboxes/Default/back.png",
};
const std::string DEFAULT_HDR_CUBEMAP = "/Skyboxes/DefaultHDR/sky.hdr";

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

class ZDomain;
class ZInput;
class ZPhysicsUniverse;
class ZAudio;
class ZIDSequence;
class ZScene;
class ZGameObject;
class ZComponent;
class ZUIElement;
class ZModel;
class ZFont;
class ZShader;
class ZMesh3D;
class ZMaterial;
class ZProcess;
class ZEvent;
class ZLight;
class ZAssetStore;
struct ZAnimation;
struct ZBone;
struct ZTexture;

using ZGameObjectMap = std::map<std::string, std::shared_ptr<ZGameObject>>;
using ZLightMap = std::map<std::string, std::shared_ptr<ZLight>>;
using ZUIElementMap = std::map<std::string, std::shared_ptr<ZUIElement>>;
using ZShaderMap = std::map<std::string, std::shared_ptr<ZShader>>;
using ZModelMap = std::map<std::string, std::shared_ptr<ZModel>>;
using ZFontMap = std::map<std::string, std::shared_ptr<ZFont>>;
using ZMaterialMap = std::map<std::string, std::shared_ptr<ZMaterial>>;
using ZShaderIDMap = std::map<std::shared_ptr<ZShader>, std::string>;
using ZModelIDMap = std::map<std::shared_ptr<ZModel>, std::string>;
using ZTextureIDMap = std::map<std::shared_ptr<ZTexture>, std::string>;
using ZFontIDMap = std::map<std::shared_ptr<ZFont>, std::string>;
using ZMaterialIDMap = std::map<std::shared_ptr<ZMaterial>, std::string>;
using ZTextureMap = std::map<std::string, std::shared_ptr<ZTexture>>;
using ZMesh3DMap = std::map<std::string, std::shared_ptr<ZMesh3D>>;
using ZGameObjectList = std::vector<std::shared_ptr<ZGameObject>>;
using ZComponentList = std::vector<std::shared_ptr<ZComponent>>;
using ZUIElementList = std::vector<std::shared_ptr<ZUIElement>>;
using ZProcessList = std::list<std::shared_ptr<ZProcess>>;
using ZCollisionPair = std::pair<ZGameObject*, ZGameObject*>;
using ZCollisionPairs = std::set<ZCollisionPair>;
using ZAnimationMap = std::map<std::string, std::shared_ptr<ZAnimation>>;
using ZBoneMap = std::map<std::string, unsigned int>;
using ZBoneList = std::vector<std::shared_ptr<ZBone>>;
using ZTimedUpdateCallback = std::function<void(float)>;
using ZTypeIdentifier = unsigned long;

enum ZPriority
{
    FirstPriority, Critical = FirstPriority, High, Medium, Normal, Low, LastPriority
};

enum ZRenderOrder
{
    First, Invisible = First, Static, Dynamic, Sky, UI, Last
};

enum class ZRenderOp
{
    Depth, Shadow, Color, Post, UI
};

enum class ZPlayState
{
    NotStarted, Loading, Ready, Playing, Paused
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
    Directional, Point, Spot, Area
};

enum class ZColliderType
{
    None, Box, Sphere, Capsule
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

enum class ZTextureWrapping
{
    Repeat, EdgeClamp, BorderClamp
};

enum class ZAlignment
{
    Left, Middle, Right, Top, Bottom
};

enum class ZPositioning
{
    Absolute, Relative
};

struct ZMaterialProperties
{
    glm::vec4 albedo;
    float alpha;
    float tiling;
    bool isPBR;
    bool hasDisplacement;
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
        tiling = 1.f;
    }
};

struct ZVertex3D
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    unsigned int boneIDs[BONES_PER_VERTEX];
    float boneWeights[BONES_PER_VERTEX];

    ZVertex3D()
    {
        for (unsigned int i = 0; i < BONES_PER_VERTEX; i++) boneWeights[i] = 0.f;
    }

    ZVertex3D(const glm::vec3& position, const glm::vec3& normal = glm::vec3(0.f, 1.f, 0.f)) : ZVertex3D()
    {
        this->position = position;
        this->normal = normal;
    }

    void AddBoneData(unsigned int boneID, float weight)
    {
        for (unsigned int i = 0; i < BONES_PER_VERTEX; i++)
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

    ZVertex2D(const glm::vec2& position = glm::vec2(0.f), const glm::vec2& uv = glm::vec2(0.f)) : position(position), uv(uv) {}
};

struct ZCharacter
{
    glm::vec2 advance;
    glm::vec2 bitmapSize;
    glm::vec2 bitmapPos;
    float xOffset;
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

struct ZDomainOptions
{
    glm::vec2 windowSize;
    glm::vec2 resolution;
    bool maximized = true;
    bool offline = false;
};

struct ZGraphicsOptions
{
    glm::vec4 clearColor{ 0.f, 0.f, 0.f, 1.f };
    bool hasPBR{ true };
    bool hasMotionBlur{ false };
    bool drawPhysicsDebug{ false };
    bool drawCameraDebug{ false };
    bool drawGrid{ false };
};

struct ZGameOptions
{
    ZDomainOptions domain;
    ZGraphicsOptions graphics;
};

struct ZInstancedDataOptions
{
    std::vector<glm::mat4> translations;
    unsigned int count = 1;
};

struct ZVertex3DDataOptions
{
    std::vector<ZVertex3D> vertices;
    std::vector<unsigned int> indices;
    ZInstancedDataOptions instanced;
};

struct ZVertex2DDataOptions
{
    std::vector<ZVertex2D> vertices;
    unsigned int numVertices;
    ZInstancedDataOptions instanced;
};

struct ZGameSystems
{
    std::shared_ptr<ZDomain> domain{ nullptr };
    std::shared_ptr<ZPhysicsUniverse> physics{ nullptr };
    std::shared_ptr<ZAudio> audio{ nullptr };
    std::shared_ptr<ZAssetStore> assetStore{ nullptr };;
};

struct ZSceneSnapshot
{
    std::shared_ptr<ZScene> scene;
    double time;
};

struct ZRect 
{
    glm::vec2 position;
    glm::vec2 size;

    ZRect() : ZRect(0.f, 0.f, 0.f, 0.f) { }
    ZRect(float val) : ZRect(val, val, val, val) { }
    ZRect(const glm::vec2& position, const glm::vec2& size) : position(position), size(size) { }
    ZRect(float x, float y, float w, float h)
    {
        position.x = x;
        position.y = y;
        size.x = w;
        size.y = h;
    }

    inline ZRect operator*(float factor) const
    {
        return ZRect(position * factor, size * factor);
    }
    inline ZRect& operator*(float factor)
    {
        position *= factor;
        size *= factor;
        return *this;
    }
    inline bool operator==(const ZRect& other) const {
        return position == other.position && size == other.size;
    }
    inline bool operator!=(const ZRect& other) const {
        return !((*this) == other);
    }
};

struct ZUITheme {
    glm::vec4                    primaryColor;
    glm::vec4                    secondaryColor;
    glm::vec4                    buttonColor;
    glm::vec4                    textColor;
    glm::vec4                    highlightColor;
    glm::vec4                    selectedColor;
    std::string                  font;
    float                        fontSize;
};

#define ID_FROM_STRING(str) std::hash<std::string>{}(str)
#define TYPE_ID(type) ID_FROM_STRING(#type)
#define SECONDS_TIME std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 0.001