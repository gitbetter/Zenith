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

#include "ZAssetLoadProgressTracker.hpp"
#include "ZZOFReadyEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZMaterialReadyEvent.hpp"
#include "ZSoundReadyEvent.hpp"
#include "ZFontReadyEvent.hpp"
#include "ZScriptReadyEvent.hpp"

void ZAssetLoadProgressTracker::TrackTexture(const ZHTexture& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingTextures);
		pendingTextureHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::TrackModel(const ZHModel& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingModels);
		pendingModelHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::TrackShader(const ZHShader& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingShaders);
		pendingShaderHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::TrackAudio(const ZHAudio& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingAudio);
		pendingAudioHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::TrackFont(const ZHFont& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingFonts);
		pendingFontHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::TrackZof(const ZHZof& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingZofs);
		pendingZofHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::TrackMaterial(const ZHMaterial& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingMaterials);
		pendingMaterialHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::TrackScript(const ZHScript& handle)
{
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingScripts);
		pendingScriptHandles_.emplace_back(handle);
		++totalTracking_;
	}
}

void ZAssetLoadProgressTracker::HandleZOFReady(const std::shared_ptr<ZZOFReadyEvent>& event)
{
	ZHZof handle = event->ZOF();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingZofs);
		auto it = std::find(pendingZofHandles_.begin(), pendingZofHandles_.end(), handle);
		if (it != pendingZofHandles_.end())
		{
			pendingZofHandles_.erase(it);
			++totalLoaded_;
		}
	}
}

void ZAssetLoadProgressTracker::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
	ZHTexture handle = event->Texture();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingTextures);
		auto it = std::find(pendingTextureHandles_.begin(), pendingTextureHandles_.end(), handle);
		if (it != pendingTextureHandles_.end())
		{
			pendingTextureHandles_.erase(it);
			++totalLoaded_;
		}
	}
}

void ZAssetLoadProgressTracker::HandleShaderReady(const std::shared_ptr<ZShaderReadyEvent>& event)
{
	ZHShader handle = event->Shader();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingShaders);
		auto it = std::find(pendingShaderHandles_.begin(), pendingShaderHandles_.end(), handle);
		if (it != pendingShaderHandles_.end())
		{
			pendingShaderHandles_.erase(it);
			++totalLoaded_;
		}
	}
}

void ZAssetLoadProgressTracker::HandleModelReady(const std::shared_ptr<ZModelReadyEvent>& event)
{
	ZHModel handle = event->Model();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingModels);
		auto it = std::find(pendingModelHandles_.begin(), pendingModelHandles_.end(), handle);
		if (it != pendingModelHandles_.end())
		{
			pendingModelHandles_.erase(it);
			++totalLoaded_;
		}
	}
}

void ZAssetLoadProgressTracker::HandleMaterialReady(const std::shared_ptr<ZMaterialReadyEvent>& event)
{
	ZHMaterial handle = event->Material();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingMaterials);
		auto it = std::find(pendingMaterialHandles_.begin(), pendingMaterialHandles_.end(), handle);
		if (it != pendingMaterialHandles_.end())
		{
			pendingMaterialHandles_.erase(it);
			++totalLoaded_;
		}
	}
}

void ZAssetLoadProgressTracker::HandleAudioReady(const std::shared_ptr<ZSoundReadyEvent>& event)
{
	ZHAudio handle = event->Audio();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingAudio);
		auto it = std::find(pendingAudioHandles_.begin(), pendingAudioHandles_.end(), handle);
		if (it != pendingAudioHandles_.end())
		{
			pendingAudioHandles_.erase(it);
			++totalLoaded_;
		}
	}
}

void ZAssetLoadProgressTracker::HandleFontReady(const std::shared_ptr<ZFontReadyEvent>& event)
{
	ZHFont handle = event->Font();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingFonts);
		auto it = std::find(pendingFontHandles_.begin(), pendingFontHandles_.end(), handle);
		if (it != pendingFontHandles_.end())
		{
			pendingFontHandles_.erase(it);
			++totalLoaded_;
		}
	}
}

void ZAssetLoadProgressTracker::HandleScriptReady(const std::shared_ptr<ZScriptReadyEvent>& event)
{
	ZHScript handle = event->Script();
	if (!handle.IsNull())
	{
		std::lock_guard<std::mutex> pendingLock(mutexes_.pendingScripts);
		auto it = std::find(pendingScriptHandles_.begin(), pendingScriptHandles_.end(), handle);
		if (it != pendingScriptHandles_.end())
		{
			pendingScriptHandles_.erase(it);
			++totalLoaded_;
		}
	}
}
