/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZAssetLoadProgressTracker.hpp

    Created by Adrian Sanchez on 09/14/2021.
    Copyright © 2021 Pervasive Sense. All rights reserved.

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

#include "ZCommon.hpp"

class ZAssetLoadProgressTracker {

public:

    ZAssetLoadProgressTracker() = default;
    ~ZAssetLoadProgressTracker() = default;

public:

    inline bool Completed() const { return totalLoaded_ == totalTracking_; }
    inline float Progress() const { return static_cast<float>(totalLoaded_.load()) / static_cast<float>(totalTracking_.load()); }
    
    void TrackTexture(const ZHTexture& handle);
    void TrackModel(const ZHModel& handle);
    void TrackShader(const ZHShader& handle);
    void TrackAudio(const ZHAudio& handle);
    void TrackFont(const ZHFont& handle);
    void TrackZof(const ZHZof& handle);
    void TrackMaterial(const ZHMaterial& handle);
    void TrackScript(const ZHScript& handle);

private:

    std::atomic<unsigned int> totalTracking_{ 0 };
    std::atomic<unsigned int> totalLoaded_{ 0 };
    std::vector<ZHTexture> pendingTextureHandles_;
    std::vector<ZHModel> pendingModelHandles_;
    std::vector<ZHShader> pendingShaderHandles_;
    std::vector<ZHAudio> pendingAudioHandles_;
    std::vector<ZHFont> pendingFontHandles_;
    std::vector<ZHZof> pendingZofHandles_;
    std::vector<ZHMaterial> pendingMaterialHandles_;
    std::vector<ZHScript> pendingScriptHandles_;

	struct
	{
		std::mutex pendingTextures;
        std::mutex pendingModels;
        std::mutex pendingShaders;
        std::mutex pendingAudio;
        std::mutex pendingFonts;
        std::mutex pendingZofs;
        std::mutex pendingMaterials;
        std::mutex pendingScripts;
	} mutexes_;

private:

	void HandleZOFReady(const std::shared_ptr<class ZZOFReadyEvent>& event);
	void HandleTextureReady(const std::shared_ptr<class ZTextureReadyEvent>& event);
	void HandleShaderReady(const std::shared_ptr<class ZShaderReadyEvent>& event);
	void HandleModelReady(const std::shared_ptr<class ZModelReadyEvent>& event);
    void HandleMaterialReady(const std::shared_ptr<class ZMaterialReadyEvent>& event);
    void HandleAudioReady(const std::shared_ptr<class ZSoundReadyEvent>& event);
    void HandleFontReady(const std::shared_ptr<class ZFontReadyEvent>& event);
    void HandleScriptReady(const std::shared_ptr<class ZScriptReadyEvent>& event);

};