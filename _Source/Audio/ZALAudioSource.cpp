/*

	 ______     ______     __   __     __     ______   __  __
	/\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
	\/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
		/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
		\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

		ZALAudioSource.cpp

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

#include "ZALAudioSource.hpp"
#include "ZResourceExtraData.hpp"
#include "ZResourceHandle.hpp"
#include "al.h"

ZALAudioSource::ZALAudioSource(std::shared_ptr<ZResourceHandle> resource) : ZAudioSource(resource) {
	source_ = 0; sampleBuffer_ = 0;
}

ZALAudioSource::~ZALAudioSource() {
	alDeleteSources(1, &source_);
	alDeleteBuffers(1, &sampleBuffer_);
}

void ZALAudioSource::Initialize() {
	ZProcess::Initialize();

	std::shared_ptr<ZSoundResourceExtraData> extra = std::static_pointer_cast<ZSoundResourceExtraData>(resourceHandle_->ExtraData());
	ALenum format, error;
	if (extra->WavFormatDesc()->channels == 2) {
		format = extra->WavFormatDesc()->blockAlign == 4 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
	} else {
		format = extra->WavFormatDesc()->blockAlign == 2 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
	}

	alGetError();
	alGenSources(1, &source_);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error generating audio source", ZERROR);
		return;
	}

	alGetError();
	alGenBuffers(1, &sampleBuffer_);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error generating audio buffer", ZERROR);
		return;
	}

	alGetError();
	alBufferData(sampleBuffer_, format, resourceHandle_->Buffer(), resourceHandle_->Size(), extra->WavFormatDesc()->samplesPerSec);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error setting up audio buffer data", ZERROR);
		return;
	}

	alGetError();
	alSourcei(source_, AL_BUFFER, sampleBuffer_);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error attaching audio buffer data to audio source", ZERROR);
		return;
	}

	if (playOnLoad_) Play(volume_, isLooping_);
}

void ZALAudioSource::Update() {
	ZProcess::Update();

	if (!IsPlaying() && !isLooping_)
		Finish();
}

void ZALAudioSource::Play(int volume, bool looping) {
	if (source_ == 0) return;

	ALenum error;

	isLooping_ = looping;
	SetVolume(volume);

	alGetError();
	alSourcei(source_, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
	alSourcePlay(source_);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error playing audio source", ZERROR);
	}
}

void ZALAudioSource::Pause() {
	if (source_ == 0) return;

	ALenum error;

	alGetError();
	alSourcePause(source_);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error pausing audio source", ZERROR);
	}
}

void ZALAudioSource::Stop() {
	if (source_ == 0) return;

	ALenum error;

	alGetError();
	alSourceStop(source_);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error stopping audio source", ZERROR);
	}
}

void ZALAudioSource::Resume() {
	if (source_ == 0) return;

	ALenum error;

	alGetError();
	alSourcePlay(source_);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error resuming audio source", ZERROR);
	}
}

void ZALAudioSource::TogglePause() {
	if (source_ == 0) return;

	ALenum error;
	ALint state;

	alGetError();
	alGetSourcei(source_, AL_SOURCE_STATE, &state);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error toggling pause on audio source", ZERROR);
		return;
	}

	if (state == AL_PAUSED) Resume();
	else Pause();
}

bool ZALAudioSource::IsPlaying() {
	if (source_ == 0) return false;

	ALenum error;
	ALint state;

	alGetError();
	alGetSourcei(source_, AL_SOURCE_STATE, &state);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error getting audio source state", ZERROR);
		return false;
	}

	return state == AL_PLAYING;
}

void ZALAudioSource::SetVolume(int volume) {
	volume_ = volume;
	if (source_ == 0) return;

	ALenum error;
	// TODO: Make a max volume property somewhere
	alGetError();
	alSourcef(source_, AL_GAIN, glm::clamp((float)volume / 100.f, 0.f, 100.f));
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error setting audio source volume", ZERROR);
		return;
	}
}

float ZALAudioSource::Progress() const {
	ALenum error;
	ALint byteOffset, sizeInBytes;

	alGetError();
	alGetBufferi(sampleBuffer_, AL_SIZE, &sizeInBytes);
	alGetSourcei(source_, AL_BYTE_OFFSET, &byteOffset);
	if ((error = alGetError()) != AL_NO_ERROR) {
		_Z("Error getting audio source playback progress", ZERROR);
		return false;
	}

	(float)byteOffset / (float)sizeInBytes;
}
