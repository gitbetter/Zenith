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
#include "ZGOFactory.hpp"
#include "ZGraphicsFactory.hpp"
#include "ZUIFactory.hpp"
#include "ZPhysicsFactory.hpp"
#include "ZScriptExports.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZScriptableProcess.hpp"
#include "ZLogger.hpp"

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
        std::shared_ptr<ZDomain> domain_ = nullptr;
        std::shared_ptr<ZGraphics> graphics_ = nullptr;
        std::shared_ptr<ZInput> input_ = nullptr;
        std::shared_ptr<ZUI> ui_ = nullptr;
        std::shared_ptr<ZPhysics> physics_ = nullptr;
        std::shared_ptr<ZAudio> audio_ = nullptr;

        std::unique_ptr<ZProcessRunner> processRunner_ = nullptr;
        std::shared_ptr<ZEventAgent> eventAgent_ = nullptr;
        std::unique_ptr<ZResourceCache> resourceCache_ = nullptr;
        std::unique_ptr<ZLuaScriptManager> scriptManager_ = nullptr;

        std::unique_ptr<ZGOFactory> gameObjectFactory_ = nullptr;
        std::unique_ptr<ZGraphicsFactory> graphicsFactory_ = nullptr;
        std::unique_ptr<ZUIFactory> uiFactory_ = nullptr;
        std::unique_ptr<ZPhysicsFactory> physicsFactory_ = nullptr;

        std::unique_ptr<ZLogger> logger_(new ZLogger);
        std::unique_ptr<ZIDSequence> idGenerator_(new ZIDSequence);
        ZEngineOptions options_;

        double deltaTime_ = 0.0;
        double lastDeltaTime_ = 0.0;
        float frameMix_ = 0.f;
    }

    // TODO: Useful to have a config file to parse for more global state info such as window dimensions
    // and maximum resource cache size

    void Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight)
    {
        ZDomainOptions options;
        options.width = windowWidth;
        options.height = windowHeight;
        Initialize(game, options);
    }

    extern void Initialize(std::shared_ptr<ZGame> game, const ZDomainOptions& domainOptions)
    {
        currentGame_ = game;

        /* ========= Process System ============ */
        processRunner_.reset(new ZProcessRunner);
        /* ===================================== */

        /* ========= Resource Cache System ============ */
        resourceCache_.reset(new ZResourceCache(100));
        resourceCache_->Initialize();
    #ifdef DEV_BUILD
        resourceCache_->RegisterResourceFile(std::shared_ptr<ZDevResourceFile>(new ZDevResourceFile(std::string(ENGINE_ROOT) + "/_Assets")));
    #else
        resourceCache_->RegisterResourceFile(std::shared_ptr<ZZipFile>(new ZZipFile(std::string(ENGINE_ROOT) + "/_Assets.zip")));
    #endif
        resourceCache_->RegisterLoader(std::shared_ptr<ZScriptResourceLoader>(new ZScriptResourceLoader));
        resourceCache_->RegisterLoader(std::shared_ptr<ZWavResourceLoader>(new ZWavResourceLoader));
        resourceCache_->RegisterLoader(std::shared_ptr<ZOggResourceLoader>(new ZOggResourceLoader));
        /* ============================================ */

        /* ========= Event System ============ */
        eventAgent_ = std::make_shared<ZEventAgent>();
        eventAgent_->Initialize();
        processRunner_->AttachProcess(eventAgent_, ZPriority::High);
        /* =================================== */

        /* ========= Scripting System ============ */
        scriptManager_.reset(new ZLuaScriptManager);
        scriptManager_->Initialize();
        ZScriptExports::Register();
        ZScriptableProcess::RegisterScriptClass();
        // We don't need to do anything with this resource. The resource loader
        // will load and execute the script for us.
        ZResource luaSetupScript("/Scripts/init.lua", ZResourceType::Script);
        resourceCache_->GetHandle(&luaSetupScript);
        /* ======================================= */

        /* ========= Windowing System ============ */
        domain_ = std::make_shared<ZDomain>(domainOptions);
        domain_->Initialize();
        /* ======================================= */

        /* ========= Graphics System ============ */
        graphics_ = std::make_shared<ZGraphics>();
        graphics_->Initialize();
        /* ====================================== */

        /* ========= Input System ============ */
        input_ = std::make_shared<ZGLInput>();
        input_->Initialize();
        processRunner_->AttachProcess(input_);
        /* =================================== */

        /* ========= UI System ============ */
        ui_ = std::make_shared<ZUI>();
        ui_->Initialize();
        /* ================================ */

        /* ========= Physics System ============ */
        physics_ = std::make_shared<ZBulletPhysics>();
        physics_->Initialize();
        processRunner_->AttachProcess(physics_);
        /* ===================================== */

        /* ========= Audio System ============ */
        audio_ = std::make_shared<ZALAudio>();
        audio_->Initialize();
        /* ===================================== */

        /* ========= Object Factories ============ */
        gameObjectFactory_.reset(new ZGOFactory);
        graphicsFactory_.reset(new ZGraphicsFactory);
        uiFactory_.reset(new ZUIFactory);
        physicsFactory_.reset(new ZPhysicsFactory);
        /* ======================================= */
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

    ZLuaScriptManager* ScriptManager()
    {
        return scriptManager_.get();
    }

    ZGOFactory* GameObjectFactory()
    {
        return gameObjectFactory_.get();
    }

    ZGraphicsFactory* GraphicsFactory()
    {
        return graphicsFactory_.get();
    }

    ZUIFactory* UIFactory()
    {
        return uiFactory_.get();
    }

    ZPhysicsFactory* PhysicsFactory()
    {
        return physicsFactory_.get();
    }

    ZLogger* Logger()
    {
        return logger_.get();
    }

    ZIDSequence* IDSequence()
    {
        return idGenerator_.get();
    }

    ZEngineOptions& Options()
    {
        return options_;
    }

    double LastDeltaTime()
    {
        return lastDeltaTime_;
    }

    double DeltaTime()
    {
        return deltaTime_;
    }

    double SecondsTime()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() * 0.001;
    }

    float FrameMix()
    {
        return frameMix_;
    }

    void Provide(std::shared_ptr<ZDomain> domain)
    {
        domain_ = domain;
        domain_->Initialize();
    }

    void Provide(std::shared_ptr<ZGraphics> graphics)
    {
        graphics_ = graphics;
        graphics_->Initialize();
    }

    void Provide(std::shared_ptr<ZInput> input)
    {
        if (input_) input_->Abort();
        input_ = input;
        processRunner_->AttachProcess(input_);
    }

    void Provide(std::shared_ptr<ZUI> ui)
    {
        ui_ = ui;
        ui_->Initialize();
    }

    void Provide(std::shared_ptr<ZPhysics> physics)
    {
        if (physics_) physics_->Abort();
        physics_ = physics;
        physics_->Initialize();
    }

    void Provide(std::shared_ptr<ZAudio> audio)
    {
        audio_ = audio;
        audio_->Initialize();
    }

    void SetDeltaTime(double deltaTime)
    {
        lastDeltaTime_ = deltaTime_;
        deltaTime_ = deltaTime;
        SetFrameMix(glm::abs(deltaTime_ - ((double) UPDATE_STEP_SIZE * (double) MAX_FIXED_UPDATE_ITERATIONS)));
    }

    void SetFrameMix(float frameMix)
    {
        frameMix_ = glm::clamp(frameMix, 0.f, 1.f);
    }

    void LoadZOF(const std::string& zofPath)
    {
        ZResource zofResource(zofPath, ZResourceType::ZOF);
        resourceCache_->RequestHandle(zofResource);
    }

    void Log(const std::string& text, ZSeverity severity)
    {
        ZLogEntry entry;
        entry.severity = severity;
        entry.text = text;
        logger_->AddEntry(entry);

    #ifdef DEV_BUILD
        switch (severity)
        {
        case ZSeverity::Info:
            std::cout << "\033[1;97m" << "[Info]: " << __FILE__ << ":" << __LINE__ << ": " << text << "\033[0m" << std::endl;
            break;
        case ZSeverity::Warning:
            std::cout << "\033[1;33m" << "[Warning]: " << __FILE__ << ":" << __LINE__ << ": " << text << "\033[0m" << std::endl;
            break;
        case ZSeverity::Error:
            std::cout << "\033[1;91m" << "[Error]: " << __FILE__ << ":" << __LINE__ << ": " << text << "\033[0m" << std::endl;
            break;
        }
    #endif
    }

    void CleanUp()
    {
        processRunner_->AbortAllProcesses(true);

        idGenerator_.reset();
        logger_.reset();
        audio_->CleanUp(); audio_.reset();
        physics_->CleanUp(); physics_.reset();
        ui_->CleanUp(); ui_.reset();
        input_.reset();
        graphics_->CleanUp(); graphics_.reset();
        domain_->CleanUp(); domain_.reset();
        resourceCache_.reset();
        eventAgent_->CleanUp();
        scriptManager_.reset();
    }

}
