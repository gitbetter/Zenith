/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEngine.hpp
 
    Created by Adrian Sanchez on 1/25/19.
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

// We'll use more of these in the future to define our graphics implementation.
// For now, we're only assuming OpenGL
#define OGL_IMP

// Includes
#include "ZCommon.hpp"

// Forward Declarations
class ZScene;
class ZGraphics;
class ZInput;
class ZDomain;
class ZUI;
class ZPhysics;
class ZAudio;
class ZIDSequence;
class ZProcessRunner;
class ZResourceCache;
class ZGOFactory;
class ZGraphicsFactory;
class ZUIFactory;
class ZPhysicsFactory;
class ZGame;
class ZEventAgent;
class ZLuaScriptManager;
class ZLogger;

// Definitions
namespace zenith {
    
    const float DEFAULT_X_RESOLUTION = 2560.f;
    const float DEFAULT_Y_RESOLUTION = 1600.f;
    const float UPDATE_STEP_SIZE = 0.017f;
    const int MAX_FIXED_UPDATE_ITERATIONS = 50;
    const unsigned int SHADOW_MAP_SIZE = 4096;
    const unsigned int CUBE_MAP_SIZE = 2048;
    const unsigned int IRRADIANCE_MAP_SIZE = 32;
    const unsigned int PREFILTER_MAP_SIZE = 128;
    const unsigned int LUT_SIZE = 512;
    const glm::vec4 WORLD_UP(0.0f, 1.0f, 0.0f, 0.0f);
    const std::vector<std::string> DEFAULT_SKYBOX_CUBEMAP{
        "Assets/Skyboxes/Default/right.png",
        "Assets/Skyboxes/Default/left.png",
        "Assets/Skyboxes/Default/up.png",
        "Assets/Skyboxes/Default/down.png",
        "Assets/Skyboxes/Default/front.png",
        "Assets/Skyboxes/Default/back.png",
    };
    const std::string DEFAULT_HDR_CUBEMAP = "Assets/Skyboxes/DefaultHDR/sky.hdr";
    
    extern void Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight);
    
    extern std::shared_ptr<ZGame> Game();
    extern ZDomain* Domain();
    extern ZGraphics* Graphics();
    extern ZInput* Input();
    extern ZUI* UI();
    extern ZPhysics* Physics();
    extern ZAudio* Audio();
    
    extern ZProcessRunner* ProcessRunner();
    extern ZEventAgent* EventAgent();
    extern ZResourceCache* ResourceCache();
    extern ZLuaScriptManager* ScriptManager();
    
    extern ZGOFactory* GameObjectFactory();
    extern ZGraphicsFactory* GraphicsFactory();
    extern ZUIFactory* UIFactory();
    extern ZPhysicsFactory* PhysicsFactory();
    
    extern ZLogger* Logger();
    extern ZIDSequence* IDSequence();
    
    extern double LastDeltaTime();
    extern double DeltaTime();
    extern double SecondsTime();
	extern float FrameMix();
    
    extern void Provide(std::shared_ptr<ZDomain> domain);
    extern void Provide(std::shared_ptr<ZGraphics> graphics);
    extern void Provide(std::shared_ptr<ZInput> input);
    extern void Provide(std::shared_ptr<ZUI> ui);
    extern void Provide(std::shared_ptr<ZPhysics> physics);
    extern void Provide(std::shared_ptr<ZAudio> audio);
    // More provide overloads for different engine subsystems (i.e. audio, animation, etc.)
    
    extern void SetDeltaTime(double deltaTime);
	extern void SetFrameMix(float frameMix);
    
    extern void LoadZOF(std::string zofPath);
	extern std::shared_ptr<ZScene> LoadScene(std::initializer_list<std::string> zofPaths);
    
    extern void Log(std::string text, unsigned int severity);
    extern void Log(const char* text, unsigned int severity);
    
    extern void CleanUp();
    
};
