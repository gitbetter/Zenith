/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEngine.cpp

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

#include "ZGame.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZGLInput.hpp"
#include "ZUI.hpp"
#include "ZBulletPhysics.hpp"
#include "ZALAudio.hpp"
#include "ZIDSequence.hpp"
#include "ZOFTree.hpp"
#include "ZOFParser.hpp"
#include "ZProcessRunner.hpp"
#include "ZResourceCache.hpp"
#include "ZScriptResourceLoader.hpp"
#include "ZWavResourceLoader.hpp"
#include "ZOggResourceLoader.hpp"
#include "ZEventAgent.hpp"
#include "ZScriptExports.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZScriptableProcess.hpp"

#ifdef DEV_BUILD
#include "ZDevResourceFile.hpp"
#else
#include "ZZipFile.hpp"
#endif

namespace zenith
{
    namespace
    {
        std::shared_ptr<ZGame> currentGame_ = nullptr;

        std::shared_ptr<ZProcessRunner> processRunner_ = nullptr;
        std::shared_ptr<ZEventAgent> eventAgent_ = nullptr;
        std::shared_ptr<ZResourceCache> resourceCache_ = nullptr;
        std::shared_ptr<ZScriptManager> scriptManager_ = nullptr;
        std::shared_ptr<ZDomain> domain_ = nullptr;
        std::shared_ptr<ZGraphics> graphics_ = nullptr;
        std::shared_ptr<ZInput> input_ = nullptr;
        std::shared_ptr<ZUI> ui_ = nullptr;
        std::shared_ptr<ZPhysics> physics_ = nullptr;
        std::shared_ptr<ZAudio> audio_ = nullptr;

        std::unique_ptr<ZIDSequence> idGenerator_(new ZIDSequence);

        ZEngineOptions options_;
    }

    // TODO: Useful to have a config file to parse for more global state info such as window dimensions
    // and maximum resource cache size

    void Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight)
    {
        ZDomainOptions options;
        options.windowSize.x = windowWidth;
        options.windowSize.y = windowHeight;
        Initialize(game, options);
    }

    extern void Initialize(std::shared_ptr<ZGame> game, const ZDomainOptions& domainOptions)
    {
        currentGame_ = game;

        /* ========= Process System ============ */
        Provide(std::make_shared<ZProcessRunner>());
        /* ===================================== */

        /* ========= Resource Cache System ============ */
        Provide(std::make_shared<ZResourceCache>(100));
        /* ============================================ */

        /* ========= Event System ============ */
        Provide(std::make_shared<ZEventAgent>());
        /* =================================== */

        /* ========= Scripting System ============ */
        Provide(std::make_shared<ZLuaScriptManager>());
        /* ======================================= */

        /* ========= Windowing System ============ */
        Provide(std::make_shared<ZDomain>(domainOptions));
        /* ======================================= */

        /* ========= Graphics System ============ */
        Provide(std::make_shared<ZGraphics>());
        /* ====================================== */

        /* ========= Input System ============ */
        Provide(std::make_shared<ZGLInput>());
        /* =================================== */

        /* ========= UI System ============ */
        Provide(std::make_shared<ZUI>());
        /* ================================ */

        /* ========= Physics System ============ */
        Provide(std::make_shared<ZBulletPhysics>());
        /* ===================================== */

        /* ========= Audio System ============ */
        Provide(std::make_shared<ZALAudio>());
        /* ===================================== */
    }

    std::shared_ptr<ZGame> Game()
    {
        return currentGame_;
    }

    ZDomain* Domain()
    {
        return domain_.get();
    }

    ZGraphics* Graphics()
    {
        return graphics_.get();
    }

    ZInput* Input()
    {
        return input_.get();
    }

    ZUI* UI()
    {
        return ui_.get();
    }

    ZPhysics* Physics()
    {
        return physics_.get();
    }

    ZAudio* Audio()
    {
        return audio_.get();
    }

    ZProcessRunner* ProcessRunner()
    {
        return processRunner_.get();
    }

    ZResourceCache* ResourceCache()
    {
        return resourceCache_.get();
    }

    ZEventAgent* EventAgent()
    {
        return eventAgent_.get();
    }

    ZScriptManager* ScriptManager()
    {
        return scriptManager_.get();
    }

    ZIDSequence* IDSequence()
    {
        return idGenerator_.get();
    }

    ZEngineOptions& Options()
    {
        return options_;
    }

    void Provide(std::shared_ptr<ZProcessRunner> processRunner)
    {
        if (processRunner_)
            processRunner_->AbortAllProcesses(true);

        processRunner_ = processRunner;
    }

    void Provide(std::shared_ptr<ZResourceCache> resourceCache)
    {
        if (resourceCache_)
            resourceCache_->CleanUp();

        resourceCache_ = resourceCache;
        resourceCache_->Initialize();
#ifdef DEV_BUILD
        resourceCache_->RegisterResourceFile(std::shared_ptr<ZDevResourceFile>(new ZDevResourceFile(std::string(ENGINE_ROOT) + "/_Assets")));
#else
        resourceCache_->RegisterResourceFile(std::shared_ptr<ZZipFile>(new ZZipFile(std::string(ENGINE_ROOT) + "/_Assets.zip")));
#endif
        resourceCache_->RegisterLoader(std::shared_ptr<ZScriptResourceLoader>(new ZScriptResourceLoader));
        resourceCache_->RegisterLoader(std::shared_ptr<ZWavResourceLoader>(new ZWavResourceLoader));
        resourceCache_->RegisterLoader(std::shared_ptr<ZOggResourceLoader>(new ZOggResourceLoader));
    }

    void Provide(std::shared_ptr<ZEventAgent> eventAgent)
    {
        if (eventAgent_) {
            eventAgent_->Abort();
            eventAgent_->CleanUp();
        }

        eventAgent_ = eventAgent;
        eventAgent_->Initialize();
        processRunner_->AttachProcess(eventAgent_, ZPriority::High);
    }

    void Provide(std::shared_ptr<ZScriptManager> scriptManager)
    {
        scriptManager_ = scriptManager;
        scriptManager_->Initialize();
        ZScriptExports::Register();
        ZScriptableProcess::RegisterScriptClass();
        // We don't need to do anything with this resource. The resource loader
        // will load and execute the script for us.
        if (resourceCache_) {
            ZResource luaSetupScript("/Scripts/init.lua", ZResourceType::Script);
            resourceCache_->GetHandle(&luaSetupScript);
        }
    }

    void Provide(std::shared_ptr<ZDomain> domain)
    {
        if (domain_)
            domain_->CleanUp();

        domain_ = domain;
        domain_->Initialize();
    }

    void Provide(std::shared_ptr<ZGraphics> graphics)
    {
        if (graphics_)
            graphics_->CleanUp();

        graphics_ = graphics;
        graphics_->Initialize();
    }

    void Provide(std::shared_ptr<ZInput> input)
    {
        if (input_) {
            input_->Abort();
            input_->CleanUp();
        }

        input_ = input;
        input_->Initialize();
        processRunner_->AttachProcess(input_);
    }

    void Provide(std::shared_ptr<ZUI> ui)
    {
        if (ui_)
            ui_->CleanUp();

        ui_ = ui;
        ui_->Initialize();
    }

    void Provide(std::shared_ptr<ZPhysics> physics)
    {
        if (physics_) {
            physics_->Abort();
            physics_->CleanUp();
        }
        if (!physics)
            return;

        physics_ = physics;
        physics_->Initialize();
        processRunner_->AttachProcess(physics_);
    }

    void Provide(std::shared_ptr<ZAudio> audio)
    {
        if (audio_)
            audio_->CleanUp();

        audio_ = audio;
        audio_->Initialize();
    }

    double SecondsTime()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() * 0.001;
    }

    void LoadZOF(const std::string& zofPath)
    {
        ZResource zofResource(zofPath, ZResourceType::ZOF);
        resourceCache_->RequestHandle(zofResource);
    }

    void CleanUp()
    {
        processRunner_->AbortAllProcesses(true);

        idGenerator_.reset();
        audio_->CleanUp(); audio_.reset();
        physics_->CleanUp(); physics_.reset();
        ui_->CleanUp(); ui_.reset();
        input_->CleanUp(); input_.reset();
        graphics_->CleanUp(); graphics_.reset();
        domain_->CleanUp(); domain_.reset();
        resourceCache_->CleanUp(); resourceCache_.reset();
        eventAgent_->CleanUp(); eventAgent_.reset();
        scriptManager_.reset();
    }

}
