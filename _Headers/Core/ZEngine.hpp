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

// Class Definitions
class ZEngine {
private:
    
    static std::shared_ptr<ZGame> currentGame_;
    static std::shared_ptr<ZDomain> domain_;
    static std::shared_ptr<ZGraphics> graphics_;
    static std::shared_ptr<ZInput> input_;
    static std::shared_ptr<ZUI> ui_;
    static std::shared_ptr<ZPhysics> physics_;
    static std::shared_ptr<ZAudio> audio_;
    
    static std::unique_ptr<ZProcessRunner> processRunner_;
    static std::shared_ptr<ZEventAgent> eventAgent_;
    static std::unique_ptr<ZResourceCache> resourceCache_;
    static std::unique_ptr<ZLuaScriptManager> scriptManager_;
    
    static std::unique_ptr<ZGOFactory> gameObjectFactory_;
    static std::unique_ptr<ZGraphicsFactory> graphicsFactory_;
    static std::unique_ptr<ZUIFactory> uiFactory_;
    static std::unique_ptr<ZPhysicsFactory> physicsFactory_;
    
    static std::unique_ptr<ZIDSequence> idGenerator_;
    static double deltaTime_, lastDeltaTime_;
    
public:
    
    static const float DEFAULT_X_RESOLUTION;
    static const float DEFAULT_Y_RESOLUTION;
    static const float UPDATE_STEP_SIZE;
    static const int MAX_FIXED_UPDATE_ITERATIONS;
    static const unsigned int SHADOW_MAP_SIZE;
    static const unsigned int CUBE_MAP_SIZE;
    static const unsigned int IRRADIANCE_MAP_SIZE;
    static const unsigned int PREFILTER_MAP_SIZE;
    static const unsigned int LUT_SIZE;
    static const glm::vec4 WORLD_UP;
    static const std::vector<std::string> DEFAULT_SKYBOX_CUBEMAP;
    static const std::string DEFAULT_HDR_CUBEMAP;
    
    static void Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight);
    
    static std::shared_ptr<ZGame> Game();
    static ZDomain* Domain();
    static ZGraphics* Graphics();
    static ZInput* Input();
    static ZUI* UI();
    static ZPhysics* Physics();
    static ZAudio* Audio();
    
    static ZProcessRunner* ProcessRunner();
    static ZEventAgent* EventAgent();
    static ZResourceCache* ResourceCache();
    static ZLuaScriptManager* ScriptManager();
    
    static ZGOFactory* GameObjectFactory();
    static ZGraphicsFactory* GraphicsFactory();
    static ZUIFactory* UIFactory();
    static ZPhysicsFactory* PhysicsFactory();
    
    static ZIDSequence* IDSequence();
    
    static double LastDeltaTime();
    static double DeltaTime();
    static double SecondsTime();
    
    static void Provide(std::shared_ptr<ZDomain> domain);
    static void Provide(std::shared_ptr<ZGraphics> graphics);
    static void Provide(std::shared_ptr<ZInput> input);
    static void Provide(std::shared_ptr<ZUI> ui);
    static void Provide(std::shared_ptr<ZPhysics> physics);
    static void Provide(std::shared_ptr<ZAudio> audio);
    // More provide overloads for different engine subsystems (i.e. audio, animation, etc.)
    
    static void SetDeltaTime(double deltaTime);
    
    static void LoadZOF(std::string zofPath);
	static std::shared_ptr<ZScene> LoadScene(std::initializer_list<std::string> zofPaths);
    
    static void CleanUp();
    
protected:
    
};
