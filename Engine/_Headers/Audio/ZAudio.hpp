/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZAudio.hpp

		Created by Adrian Sanchez on 13/04/2019.
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
#include "ZCommon.hpp"

// Forward Declarations
class ZAudioSource;
class ZResourceHandle;

// Class and Data Structure Definitions
class ZAudio {

public:

	ZAudio() : allPaused_(false), initialized_(false) { }
	virtual ~ZAudio() { }

	virtual bool Initialize() = 0;

	virtual bool Active() = 0;
	virtual std::shared_ptr<ZAudioSource> NewAudioSource(std::shared_ptr<ZResourceHandle> resource, bool playOnLoad = false) = 0;
	virtual bool ReleaseAudioSource(std::shared_ptr<ZAudioSource> audioSource) = 0;
	virtual void CleanUp();

	bool Paused() { return allPaused_; }
	bool Initialized() { return initialized_; }

  void StopAll();
	void PauseAll();
	void ResumeAll();

protected:

	typedef std::list<std::shared_ptr<ZAudioSource>> ZAudioSourceList;

	ZAudioSourceList allSamples_;
	bool allPaused_, initialized_;

};

