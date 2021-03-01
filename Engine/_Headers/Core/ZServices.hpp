/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZServices.hpp

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

#pragma once

// Includes
#include "ZProcessRunner.hpp"
#include "ZResourceCache.hpp"
#include "ZEventAgent.hpp"
#include "ZScriptManager.hpp"
#include "ZAssetStore.hpp"
#include "ZGraphics.hpp"
#include "ZInput.hpp"
#include "ZLogger.hpp"

// Definitions
class ZServices
{

public:

    static void Initialize();

    static std::shared_ptr<ZGraphics> Graphics() { return graphics_; }
    static std::shared_ptr<ZInput> Input() { return input_; }
    static std::shared_ptr<ZEventAgent> EventAgent() { return eventAgent_; }
    static std::shared_ptr<ZResourceCache> ResourceCache() { return resourceCache_; }
    static std::shared_ptr<ZScriptManager> ScriptManager() { return scriptManager_; }
    static std::shared_ptr<ZProcessRunner> ProcessRunner(const std::string& runner = "Default");
    static std::shared_ptr<ZLogger> Logger(const std::string& logger = "Default");
    static std::shared_ptr<ZAssetStore> AssetStore();

    static void Provide(const std::shared_ptr<ZGraphics>& graphics);
    static void Provide(const std::shared_ptr<ZInput>& input);
    static void Provide(const std::shared_ptr<ZResourceCache>& resourceCache);
    static void Provide(const std::shared_ptr<ZEventAgent>& eventAgent);
    static void Provide(const std::shared_ptr<ZScriptManager>& scriptManager);
    static void Provide(const std::shared_ptr<ZAssetStore>& assetStore);

    static void LoadZOF(const std::string& zofPath);

private:

    static std::shared_ptr<ZGraphics> graphics_;
    static std::shared_ptr<ZInput> input_;
    static std::shared_ptr<ZEventAgent> eventAgent_;
    static std::shared_ptr<ZResourceCache> resourceCache_;
    static std::shared_ptr<ZScriptManager> scriptManager_;
    static std::shared_ptr<ZAssetStore> assetStore_;
    static std::unordered_map<std::string, std::shared_ptr<ZProcessRunner>> processRunners_;
    static std::unordered_map<std::string, std::shared_ptr<ZLogger>> loggers_;
    static bool initialized_;

};

#define LOG(message, severity) ZServices::Logger(std::filesystem::path(__FILE__).stem().string())->Log(message, severity, __FILE__, __LINE__);
#define ILOG(message) LOG(message, ZSeverity::Info);