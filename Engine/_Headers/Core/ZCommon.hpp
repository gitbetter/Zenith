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
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include <initializer_list>
#include <array>
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
#include "ZIDSequence.hpp"
#include "ZStringHelpers.hpp"
#include "ZFrameProfiler.hpp"

constexpr unsigned int BONES_PER_VERTEX = 4;
constexpr unsigned int BONES_PER_MODEL = 50;
constexpr unsigned int MAX_MATERIALS_PER_OBJECT = 6;
constexpr unsigned int MAX_LOCAL_LIGHTS = 4;
constexpr unsigned int NUM_SHADOW_CASCADES = 4;
constexpr unsigned int MAX_TEXTURE_SLOTS = 16;
constexpr unsigned int MAX_UBO_SLOTS = 16;

constexpr float UPDATE_STEP_SIZE = 0.017f;
constexpr int MAX_FIXED_UPDATE_ITERATIONS = 4;
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

const float EPSILON = glm::epsilon<float>();
const float PI = glm::pi<float>();

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

const std::unordered_map<ZKey, std::string> KEY_CODE{
    {ZKey::UNKNOWN, "unknown"},
    {ZKey::SPACE, " "},
    {ZKey::APOSTROPHE, "'"},
    {ZKey::COMMA, ","},
    {ZKey::MINUS, "-"},
    {ZKey::PERIOD, "."},
    {ZKey::SLASH, "/"},
    {ZKey::N0, "0"},
    {ZKey::N1, "1"},
    {ZKey::N2, "2"},
    {ZKey::N3, "3"},
    {ZKey::N4, "4"},
    {ZKey::N5, "5"},
    {ZKey::N6, "6"},
    {ZKey::N7, "7"},
    {ZKey::N8, "8"},
    {ZKey::N9, "9"},
    {ZKey::SEMICOLON, ";"},
    {ZKey::EQUAL, "="},
    {ZKey::A, "a"},
    {ZKey::B, "b"},
    {ZKey::C, "c"},
    {ZKey::D, "d"},
    {ZKey::E, "e"},
    {ZKey::F, "f"},
    {ZKey::G, "g"},
    {ZKey::H, "h"},
    {ZKey::I, "i"},
    {ZKey::J, "j"},
    {ZKey::K, "k"},
    {ZKey::L, "l"},
    {ZKey::M, "m"},
    {ZKey::N, "n"},
    {ZKey::O, "o"},
    {ZKey::P, "p"},
    {ZKey::Q, "q"},
    {ZKey::R, "r"},
    {ZKey::S, "s"},
    {ZKey::T, "t"},
    {ZKey::U, "u"},
    {ZKey::V, "v"},
    {ZKey::W, "w"},
    {ZKey::X, "x"},
    {ZKey::Y, "y"},
    {ZKey::Z, "z"},
    {ZKey::LEFT_BRACKET, "{"},
    {ZKey::BACKSLASH, "\\"},
    {ZKey::RIGHT_BRACKET, "}"},
    {ZKey::GRAVE_ACCENT, "`"},
    {ZKey::ESCAPE, "escape"},
    {ZKey::ENTER, "enter"},
    {ZKey::TAB, "    "},
    {ZKey::BACKSPACE, "backspace"},
    {ZKey::INSERT, "insert"},
    {ZKey::DELETE, "delete"},
    {ZKey::RIGHT, "right"},
    {ZKey::LEFT, "left"},
    {ZKey::DOWN, "down"},
    {ZKey::UP, "up"},
    {ZKey::PAGE_UP, "page_up"},
    {ZKey::PAGE_DOWN, "page_down"},
    {ZKey::HOME, "home"},
    {ZKey::END, "end"},
    {ZKey::CAPS_LOCK, "caps_lock"},
    {ZKey::SCROLL_LOCK, "scroll_lock"},
    {ZKey::NUM_LOCK, "num_lock"},
    {ZKey::PRINT_SCREEN, "print_screen"},
    {ZKey::PAUSE, "pause"},
    {ZKey::F1, "f1"},
    {ZKey::F2, "f2"},
    {ZKey::F3, "f3"},
    {ZKey::F4, "f4"},
    {ZKey::F5, "f5"},
    {ZKey::F6, "f6"},
    {ZKey::F7, "f7"},
    {ZKey::F8, "f8"},
    {ZKey::F9, "f9"},
    {ZKey::F10, "f10"},
    {ZKey::F11, "f11"},
    {ZKey::F12, "f12"},
    {ZKey::F13, "f13"},
    {ZKey::F14, "f14"},
    {ZKey::F15, "f15"},
    {ZKey::F16, "f16"},
    {ZKey::F17, "f17"},
    {ZKey::F18, "f18"},
    {ZKey::F19, "f19"},
    {ZKey::F20, "f20"},
    {ZKey::F21, "f21"},
    {ZKey::F22, "f22"},
    {ZKey::F23, "f23"},
    {ZKey::F24, "f24"},
    {ZKey::F25, "f25"},
    {ZKey::KP_0, "kp_0"},
    {ZKey::KP_1, "kp_1"},
    {ZKey::KP_2, "kp_2"},
    {ZKey::KP_3, "kp_3"},
    {ZKey::KP_4, "kp_4"},
    {ZKey::KP_5, "kp_5"},
    {ZKey::KP_6, "kp_6"},
    {ZKey::KP_7, "kp_7"},
    {ZKey::KP_8, "kp_8"},
    {ZKey::KP_9, "kp_9"},
    {ZKey::KP_DECIMAL, "kp_decimal"},
    {ZKey::KP_DIVIDE, "kp_divide"},
    {ZKey::KP_MULTIPLY, "kp_multiply"},
    {ZKey::KP_SUBTRACT, "kp_subtract"},
    {ZKey::KP_ADD, "kp_add"},
    {ZKey::KP_ENTER, "kp_enter"},
    {ZKey::KP_EQUAL, "kp_equal"},
    {ZKey::LEFT_SHIFT, "left_shift"},
    {ZKey::LEFT_CONTROL, "left_control"},
    {ZKey::LEFT_ALT, "left_alt"},
    {ZKey::LEFT_SUPER, "left_super"},
    {ZKey::RIGHT_SHIFT, "right_shift"},
    {ZKey::RIGHT_CONTROL, "right_control"},
    {ZKey::RIGHT_ALT, "right_alt"},
    {ZKey::RIGHT_SUPER, "right_super"},
    {ZKey::MENU, "menu"}
};

const std::unordered_map<ZKey, char> PRINTABLE_KEY_CODE{
    {ZKey::SPACE, ' '},
    {ZKey::APOSTROPHE, '\''},
    {ZKey::COMMA, ','},
    {ZKey::MINUS, '-'},
    {ZKey::PERIOD, '.'},
    {ZKey::SLASH, '/'},
    {ZKey::N0, '0'},
    {ZKey::N1, '1'},
    {ZKey::N2, '2'},
    {ZKey::N3, '3'},
    {ZKey::N4, '4'},
    {ZKey::N5, '5'},
    {ZKey::N6, '6'},
    {ZKey::N7, '7'},
    {ZKey::N8, '8'},
    {ZKey::N9, '9'},
    {ZKey::SEMICOLON, ';'},
    {ZKey::EQUAL, '='},
    {ZKey::A, 'a'},
    {ZKey::B, 'b'},
    {ZKey::C, 'c'},
    {ZKey::D, 'd'},
    {ZKey::E, 'e'},
    {ZKey::F, 'f'},
    {ZKey::G, 'g'},
    {ZKey::H, 'h'},
    {ZKey::I, 'i'},
    {ZKey::J, 'j'},
    {ZKey::K, 'k'},
    {ZKey::L, 'l'},
    {ZKey::M, 'm'},
    {ZKey::N, 'n'},
    {ZKey::O, 'o'},
    {ZKey::P, 'p'},
    {ZKey::Q, 'q'},
    {ZKey::R, 'r'},
    {ZKey::S, 's'},
    {ZKey::T, 't'},
    {ZKey::U, 'u'},
    {ZKey::V, 'v'},
    {ZKey::W, 'w'},
    {ZKey::X, 'x'},
    {ZKey::Y, 'y'},
    {ZKey::Z, 'z'},
    {ZKey::LEFT_BRACKET, '{'},
    {ZKey::BACKSLASH, '\\'},
    {ZKey::RIGHT_BRACKET, '}'},
    {ZKey::GRAVE_ACCENT, '`'},
    {ZKey::TAB, '    '},
    {ZKey::KP_0, '0'},
    {ZKey::KP_1, '1'},
    {ZKey::KP_2, '2'},
    {ZKey::KP_3, '3'},
    {ZKey::KP_4, '4'},
    {ZKey::KP_5, '5'},
    {ZKey::KP_6, '6'},
    {ZKey::KP_7, '7'},
    {ZKey::KP_8, '8'},
    {ZKey::KP_9, '9'},
    {ZKey::KP_DECIMAL, '.'},
    {ZKey::KP_DIVIDE, '/'},
    {ZKey::KP_MULTIPLY, '*'},
    {ZKey::KP_SUBTRACT, '-'},
    {ZKey::KP_ADD, '+'},
    {ZKey::KP_EQUAL, '='}
};

enum class ZMouse
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
class ZTexture;
struct ZCursor;
struct ZVertex3D;
struct ZVertex2D;
class ZColliderComponent;
class ZScriptComponent;
class ZUniformBuffer;

using ZGameObjectMap = std::unordered_map<std::string, std::shared_ptr<ZGameObject>>;
using ZLightMap = std::unordered_map<std::string, std::shared_ptr<ZLight>>;
using ZUIElementMap = std::unordered_map<std::string, std::shared_ptr<ZUIElement>>;
using ZShaderMap = std::unordered_map<std::string, std::shared_ptr<ZShader>>;
using ZModelMap = std::unordered_map<std::string, std::shared_ptr<ZModel>>;
using ZFontMap = std::unordered_map<std::string, std::shared_ptr<ZFont>>;
using ZMaterialMap = std::unordered_map<std::string, std::shared_ptr<ZMaterial>>;
using ZShaderIDMap = std::unordered_map<std::shared_ptr<ZShader>, std::string>;
using ZModelIDMap = std::unordered_map<std::shared_ptr<ZModel>, std::string>;
using ZTextureIDMap = std::unordered_map<std::shared_ptr<ZTexture>, std::string>;
using ZFontIDMap = std::unordered_map<std::shared_ptr<ZFont>, std::string>;
using ZMaterialIDMap = std::unordered_map<std::shared_ptr<ZMaterial>, std::string>;
using ZTextureMap = std::unordered_map<std::string, std::shared_ptr<ZTexture>>;
using ZMesh3DMap = std::unordered_map<std::string, std::shared_ptr<ZMesh3D>>;
using ZGameObjectList = std::vector<std::shared_ptr<ZGameObject>>;
using ZLightList = std::vector<std::shared_ptr<ZLight>>;
using ZComponentList = std::vector<std::shared_ptr<ZComponent>>;
using ZUIElementList = std::vector<std::shared_ptr<ZUIElement>>;
using ZProcessList = std::list<std::shared_ptr<ZProcess>>;
using ZCollisionPair = std::pair<ZGameObject*, ZGameObject*>;
using ZCollisionPairs = std::set<ZCollisionPair>;
using ZAnimationMap = std::map<std::string, std::shared_ptr<ZAnimation>>;
using ZBoneMap = std::map<std::string, unsigned int>;
using ZIDMap = std::unordered_map<std::string, unsigned int>;
using ZBoneList = std::vector<std::shared_ptr<ZBone>>;
using ZMaterialList = std::vector<std::shared_ptr<ZMaterial>>;
using ZTextureList = std::array<std::shared_ptr<ZTexture>, MAX_TEXTURE_SLOTS>;
using ZUBOList = std::array<std::shared_ptr<ZUniformBuffer>, MAX_UBO_SLOTS>;
using ZTimedUpdateCallback = std::function<void(float)>;
using ZTypeIdentifier = unsigned long;
using ZVertex3DList = std::vector<ZVertex3D>;
using ZVertex2DList = std::vector<ZVertex2D>;

enum ZPriority
{
    FirstPriority, Critical = FirstPriority, High, Medium, Normal, Low, LastPriority
};

enum class ZRenderLayer
{
    Null = 0, First, Invisible = First, Static, Dynamic, Debug, Sky, UI, Last
};

enum class ZFullScreenLayer
{
    Null = 0, First, Game = First, Effect, UI, Last
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
    Dynamic = 1, Static = 2, Kinematic = 4, Particle = 8, Trigger = 16, Character = 32, All = -1
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
    Point = 1, Spot, Directional, Area
};

enum class ZColliderType
{
    None, Box, Sphere, Capsule, Cylinder, Cone
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
    Canvas, Button, CheckBox, Image, InputField, ListPanel, Panel, Text, Unknown
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

enum class ZSystemCursorType
{
    Arrow, Caret, Crosshair, Hand, HorizontalResize, VerticalResize
};

enum class ZTextureBindPoint
{
    Depth = 0, Shadow, Color, Irradiance, Prefilter, BRDFLUT, User0
};

enum class ZClearFlags
{
    Null = 0, None, Color, Stencil = 4, Depth = 8
};

enum class ZBlendMode
{
    Null = 0, None, Opaque, Transluscent, Additive
};

enum class ZDepthStencilState
{
    Null = 0, None, Depth, Stencil = 4
};

enum class ZFaceCullState
{
    Null = 0, None, Front, Back = 4
};

struct Light {
    glm::vec4 ambient{ 0.1f };
    glm::vec4 color{ 0.9f };
    glm::vec4 position{ 0.f };
    glm::vec4 direction{ 0.f };
    glm::vec4 coneDirection{ 0.f };
    float constantAttenuation{ 0.f };
    float linearAttenuation{ 0.f };
    float quadraticAttenuation{ 0.f };
    float spotCutoff{ 0.f };
    float spotExponent{ 0.f };
    alignas(sizeof(float)) bool isEnabled{ true };
    alignas(sizeof(float)) unsigned int lightType;
};

struct Material {
    glm::vec4 albedo;
    float emission;
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

struct PBRMaterial {
    glm::vec4 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct ZVertex3D
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    int boneIDs[BONES_PER_VERTEX];
    float boneWeights[BONES_PER_VERTEX];

    ZVertex3D()
    {
        for (unsigned int i = 0; i < BONES_PER_VERTEX; i++) {
            boneIDs[i] = -1;
            boneWeights[i] = 0.f;
        }
    }

    ZVertex3D(const glm::vec3& position, const glm::vec3& normal = glm::vec3(0.f, 1.f, 0.f), const glm::vec2& uv = glm::vec2(0.f)) : ZVertex3D()
    {
        this->position = position;
        this->normal = normal;
        this->uv = uv;
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

    static void ComputeTangentBitangent(ZVertex3D& v1, ZVertex3D& v2, ZVertex3D& v3)
    {
        glm::vec3 deltaPos1 = v2.position - v1.position;
        glm::vec3 deltaPos2 = v3.position - v1.position;
        glm::vec2 deltaUV1 = v2.uv - v1.uv;
        glm::vec2 deltaUV2 = v3.uv - v1.uv;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        v1.tangent = tangent; v2.tangent = tangent; v3.tangent = tangent;
        v1.bitangent = bitangent; v2.bitangent = bitangent; v3.bitangent = bitangent;
    }

    static glm::vec3 ComputeFaceNormal(const ZVertex3D& v1, const ZVertex3D& v2, const ZVertex3D& v3)
    {
        glm::vec3 normal(0.f);

        glm::vec3 e1 = v1.position - v2.position;
        glm::vec3 e2 = v1.position - v3.position;

        glm::vec3 e1crosse2 = glm::cross(e1, e2);

        if (glm::length(e1crosse2) > EPSILON)
        {
            normal = glm::normalize(e1crosse2);
        }

        return normal;
    }
};

struct ZVertex2D
{
    glm::vec4 vertex;

    ZVertex2D(float x = 0.f, float y = 0.f, float u = 0.f, float v = 0.f)
        : vertex(x, y, u, v)
    {}

    static ZVertex2DList GenerateUnitCircleVertices(int segments)
    {
        float sectorStep = 2 * PI / segments;
        float sectorAngle;  // radian

        ZVertex2DList vertices;
        for (int i = 0; i <= segments; ++i)
        {
            sectorAngle = i * sectorStep;
            vertices.emplace_back(cos(sectorAngle), sin(sectorAngle));
        }

        return vertices;
    }
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
    ZGameObjectList gameObjects;
    ZUIElementList uiElements;
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
    bool drawAABBDebug{ false };
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
    ZVertex3DList vertices;
    std::vector<unsigned int> indices;
    ZInstancedDataOptions instanced;
};

struct ZVertex2DDataOptions
{
    ZVertex2DList vertices;
    unsigned int numVertices;
    ZInstancedDataOptions instanced;
};

struct ZGameSystems
{
    std::shared_ptr<ZDomain> domain{ nullptr };
    std::shared_ptr<ZPhysicsUniverse> physics{ nullptr };
    std::shared_ptr<ZAudio> audio{ nullptr };
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

    inline bool Contains(const glm::vec2& point) const
    {
        return point.x >= position.x && point.x <= position.x + size.x &&
            point.y >= position.y && point.y <= position.y + size.y;
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

struct ZBasis {
    glm::vec3 x, y, z;
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

struct ZCursor {
    ZSystemCursorType type;
    ZCursor(const ZSystemCursorType& type)
        : type(type)
    { }
};

struct ZUIUniforms
{
    glm::mat4 P;
    glm::mat4 M;
    glm::vec4 color;
    glm::vec4 borderColor;
    glm::vec2 pixelSize;
    float borderWidth;
    float borderRadius;
    alignas(sizeof(float)) bool instanced;
};

struct ZCameraUniforms
{
    glm::mat4 V;
    glm::mat4 P;
    glm::mat4 ViewProjection;
    glm::mat4 PreviousViewProjection;
    glm::mat4 InverseViewProjection;
    alignas(sizeof(float) * 4) glm::vec3 ViewPosition;
};

struct ZLightUniforms
{
    glm::mat4 ViewProjectionLightSpace;
    glm::mat4 ViewProjectionsLightSpace[NUM_SHADOW_CASCADES];
    glm::vec4 shadowFarPlanes;
    Light light;
};

struct ZObjectUniforms
{
    glm::mat4 M;
};

struct ZModelUniforms
{
    glm::mat4 bones[BONES_PER_MODEL];
    alignas(sizeof(float)) bool rigged;
    alignas(sizeof(float)) bool instanced;
};

struct ZPostUniforms
{
    alignas(sizeof(float)) bool useMotionBlur;
};

struct ZMaterialUniforms
{
    Material material;
    PBRMaterial pbrMaterial;
    alignas(sizeof(float)) bool isTextured;
    alignas(sizeof(float)) bool hasDisplacement;
};

// TODO: Is there a better way we can write this type trait?
template<typename T>
struct is_multiple_components_supported {
    static constexpr bool value = false;
};
template<>
struct is_multiple_components_supported<ZColliderComponent> {
    static constexpr bool value = true;
};
template<>
struct is_multiple_components_supported<ZScriptComponent> {
    static constexpr bool value = true;
};

#if defined _WIN64 || defined _WIN32
#define NEWLINE (char)'\r\n'
#else
#define NEWLINE (char)'\n'
#endif

#define ID_FROM_STRING(str) std::hash<std::string>{}(str)
#define TYPE_ID(type) ID_FROM_STRING(#type)
#define SECONDS_TIME std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 0.001

#ifdef DEV_BUILD
#define ZPR_SESSION_BEGIN(name) ZFrameProfiler::StartSession(name);
#define ZPR_SESSION_END() ZFrameProfiler::EndSession();
#define ZPR_SESSION_COLLECT_VERTICES(vertices) ZFrameProfiler::CollectSessionVertices(vertices);
#define ZPR_SESSION_COLLECT_DRAWS(draws) ZFrameProfiler::CollectSessionDrawCalls(draws);
#define ZPR_SESSION_COLLECT_RENDER_PASSES(passes) ZFrameProfiler::CollectSessionRenderPasses(passes);
#define ZPR_SESSION(name) ZFrameProfiler::GetSession(name)
#define ZPR_SESSION_STATS(name) ZFrameProfiler::GetSessionStats(name)
#define ZPR_CURRENT_SESSION ZFrameProfiler::CurrentSession()
#else
#define ZPR_SESSION_BEGIN() ((void)0);
#define ZPR_SESSION_END() ((void)0);
#define ZPR_SESSION_COLLECT_VERTICES(vertices) ((void)0);
#define ZPR_SESSION_COLLECT_DRAWS(draws) ((void)0);
#define ZPR_SESSION_COLLECT_RENDER_PASSES(passes) ((void)0);
#define ZPR_SESSION(name) ((void)0)
#define ZPR_SESSION_STATS(name) ((void)0)
#define ZPR_CURRENT_SESSION ((void)0)
#endif