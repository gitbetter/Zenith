/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZServices.cpp

    Created by Adrian Sanchez on 27/01/2021.
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

#include "ZServices.hpp"
#include "ZOFParser.hpp"
#include "ZScriptResourceLoader.hpp"
#include "ZWavResourceLoader.hpp"
#include "ZOggResourceLoader.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZGLGraphics.hpp"
#include "ZGLInput.hpp"

#ifdef DEV_BUILD
#include "ZDevResourceFile.hpp"
#else
#include "ZZipFile.hpp"
#endif

std::shared_ptr<ZGraphics> ZServices::graphics_ = nullptr;
std::shared_ptr<ZInput> ZServices::input_ = nullptr;
std::shared_ptr<ZEventAgent> ZServices::eventAgent_ = nullptr;
std::shared_ptr<ZResourceCache> ZServices::resourceCache_ = nullptr;
std::shared_ptr<ZScriptManager> ZServices::scriptManager_ = nullptr;
std::shared_ptr<ZAssetStore> ZServices::assetStore_ = nullptr;
std::unordered_map<std::string, std::shared_ptr<ZProcessRunner>> ZServices::processRunners_;
std::unordered_map<std::string, std::shared_ptr<ZLogger>> ZServices::loggers_;
bool ZServices::initialized_ = false;

void ZServices::Initialize()
{
    if (!initialized_) {
        /* ========= Resource Cache System ============ */
        Provide(std::make_shared<ZResourceCache>(256));
        /* ============================================ */

        /* ========= Event System ============ */
        Provide(std::make_shared<ZEventAgent>());
        /* =================================== */

        /* ========= Scripting System ============ */
        Provide(std::make_shared<ZLuaScriptManager>());
        /* ======================================= */

        /* ========= Graphics System ============ */
        Provide(std::make_shared<ZGLGraphics>());
        /* ====================================== */

        /* ========= Input System ============ */
        Provide(std::make_shared<ZGLInput>());
        /* =================================== */
    }
    initialized_ = true;
}

std::shared_ptr<ZProcessRunner> ZServices::ProcessRunner(const std::string& runner)
{
    if (processRunners_.find(runner) == processRunners_.end()) {
        processRunners_[runner] = std::make_shared<ZProcessRunner>();
    }
    return processRunners_[runner];
}

std::shared_ptr<ZLogger> ZServices::Logger(const std::string& logger)
{
    if (loggers_.find(logger) == loggers_.end()) {
        loggers_[logger] = std::make_shared<ZLogger>();
    }
    return loggers_[logger];
}

std::shared_ptr<ZAssetStore> ZServices::AssetStore()
{
    // Lazy initialization since this system is dependent on a domain being present first.
    // NOTE: HARDER TO DEBUG!
    if (!assetStore_) {
        Provide(std::make_shared<ZAssetStore>());
    }
    return assetStore_;
}

void ZServices::Provide(const std::shared_ptr<ZGraphics>& graphics)
{
    graphics_ = graphics;
    graphics_->Initialize();
}

void ZServices::Provide(const std::shared_ptr<ZInput>& input)
{
    if (input_) {
        input_->Abort();
        input_->CleanUp();
    }

    input_ = input;
    input_->Initialize();
    ZServices::ProcessRunner()->AttachProcess(input_);
}

void ZServices::Provide(const std::shared_ptr<ZResourceCache>& resourceCache)
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

void ZServices::Provide(const std::shared_ptr<ZEventAgent>& eventAgent)
{
    if (eventAgent_) {
        eventAgent_->Abort();
        eventAgent_->CleanUp();
    }

    eventAgent_ = eventAgent;
    eventAgent_->Initialize();
    ZServices::ProcessRunner()->AttachProcess(eventAgent_, ZPriority::High);
}

void ZServices::Provide(const std::shared_ptr<ZScriptManager>& scriptManager)
{
    scriptManager_ = scriptManager;
    scriptManager_->Initialize();
    // We don't need to do anything with this resource. The resource loader
    // will load and execute the script for us.
    if (resourceCache_) {
        ZResource luaSetupScript("/Scripts/init.lua", ZResourceType::Script);
        resourceCache_->GetHandle(&luaSetupScript);
    }
}

void ZServices::Provide(const std::shared_ptr<ZAssetStore>& assetStore)
{
    if (assetStore_)
        assetStore_->CleanUp();

    assetStore_ = assetStore;
    assetStore_->Initialize();
}

void ZServices::LoadZOF(const std::string& zofPath)
{
    ZResource zofResource(zofPath, ZResourceType::ZOF);
    ZServices::ResourceCache()->RequestHandle(zofResource);
}