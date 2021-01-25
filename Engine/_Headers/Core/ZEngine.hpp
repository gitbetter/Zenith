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
class ZGame;
class ZEventAgent;
class ZScriptManager;

// Definitions
namespace zenith
{

    const float DEFAULT_X_RESOLUTION = 2560.f;
    const float DEFAULT_Y_RESOLUTION = 1600.f;
    const float UPDATE_STEP_SIZE = 0.017f;
    const int MAX_FIXED_UPDATE_ITERATIONS = 24;
    const unsigned int SHADOW_MAP_SIZE = 4096;
    const unsigned int CUBE_MAP_SIZE = 2048;
    const unsigned int IRRADIANCE_MAP_SIZE = 32;
    const unsigned int PREFILTER_MAP_SIZE = 128;
    const unsigned int LUT_SIZE = 512;
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

    extern void Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight);
    extern void Initialize(std::shared_ptr<ZGame> game, const ZDomainOptions& domainOptions);

    extern std::shared_ptr<ZGame> Game();

    extern ZProcessRunner* ProcessRunner();
    extern ZEventAgent* EventAgent();
    extern ZResourceCache* ResourceCache();
    extern ZScriptManager* ScriptManager();
    extern ZDomain* Domain();
    extern ZGraphics* Graphics();
    extern ZInput* Input();
    extern ZUI* UI();
    extern ZPhysics* Physics();
    extern ZAudio* Audio();

    extern ZIDSequence* IDSequence();
    extern ZEngineOptions& Options();

    extern double SecondsTime();

    extern void Provide(std::shared_ptr<ZProcessRunner> processRunner);
    extern void Provide(std::shared_ptr<ZResourceCache> resourceCache);
    extern void Provide(std::shared_ptr<ZEventAgent> eventAgent);
    extern void Provide(std::shared_ptr<ZScriptManager> scriptManager);
    extern void Provide(std::shared_ptr<ZDomain> domain);
    extern void Provide(std::shared_ptr<ZGraphics> graphics);
    extern void Provide(std::shared_ptr<ZInput> input);
    extern void Provide(std::shared_ptr<ZUI> ui);
    extern void Provide(std::shared_ptr<ZPhysics> physics);
    extern void Provide(std::shared_ptr<ZAudio> audio);

    extern void LoadZOF(const std::string& zofPath);

    extern void CleanUp();

    template <class T, typename... Args>
    std::shared_ptr<T> LoadScene(Args&&... args)
    {
        if (!std::is_base_of<ZScene, T>::value)
        {
            Log("Cannot load " + std::string(typeid(T).name()) + " as a ZScene object", ZSeverity::Error);
            return nullptr;
        }
        std::shared_ptr<T> scene = std::make_shared<T>(std::forward<Args>(args)...);
        scene->Initialize();
        Game()->AddScene(scene);
        return scene;
    }
};
