/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZALAudioSource.hpp

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
#include "ZAudioSource.hpp"

// Forward Declarations
class ZResourceHandle;

// Class and Data Structure Definitions
class ZALAudioSource : public ZAudioSource {

public:

	ZALAudioSource(std::shared_ptr<ZResourceHandle> resource = nullptr);
	~ZALAudioSource();

	void Initialize() override;
	void Update() override;
	void Play(int volume, bool looping = true) override;
	void Pause() override;
	void Stop() override;
	void Resume() override;

	void TogglePause() override;
	bool IsPlaying() override;
	void SetVolume(int volume) override;
	float Progress() const override;

private:

	unsigned int source_, sampleBuffer_;

};

