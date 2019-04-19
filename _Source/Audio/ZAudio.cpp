/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZAudio.cpp

		Created by Adrian Sanchez on 13/04/2019.
		Copyright � 2019 Pervasive Sense. All rights reserved.

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

#include "ZAudio.hpp"
#include "ZAudioSource.hpp"

void ZAudio::StopAll() {
	std::shared_ptr<ZAudioSource> audioSource = nullptr;
	for (ZAudioSourceList::iterator it = allSamples_.begin(), end = allSamples_.end(); it != end; it++) {
		audioSource = (*it);
		audioSource->Stop();
	}
	allPaused_ = false;
}

void ZAudio::PauseAll() {
	std::shared_ptr<ZAudioSource> audioSource = nullptr;
	for (ZAudioSourceList::iterator it = allSamples_.begin(), end = allSamples_.end(); it != end; it++) {
		audioSource = (*it);
		audioSource->Pause();
	}
	allPaused_ = true;
}

void ZAudio::ResumeAll() {
	std::shared_ptr<ZAudioSource> audioSource = nullptr;
	for (ZAudioSourceList::iterator it = allSamples_.begin(), end = allSamples_.end(); it != end; it++) {
		audioSource = (*it);
		audioSource->Resume();
	}
	allPaused_ = false;
}

void ZAudio::CleanUp() {
	while (!allSamples_.empty()) {
		std::shared_ptr<ZAudioSource> audioSource = allSamples_.front();
		audioSource->Stop();
		allSamples_.pop_front();
	}
}
