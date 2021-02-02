/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZALAudio.cpp

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

#include "ZALAudio.hpp"
#include "ZALAudioSource.hpp"
#include "ZServices.hpp"

bool ZALAudio::Initialize()
{
    device_ = alcOpenDevice(NULL);
    if (!device_)
    {
        LOG("Could not initialize OpenAL audio device", ZSeverity::Error);
        return false;
    }

    context_ = alcCreateContext(device_, NULL);
    alcMakeContextCurrent(context_);

    alGetError();	// Clears any OpenAL error code
    return true;
}

bool ZALAudio::Active()
{
    return context_ != nullptr;
}

std::shared_ptr<ZAudioSource> ZALAudio::NewAudioSource(std::shared_ptr<ZResourceHandle> resource, bool playOnLoad)
{
    std::shared_ptr<ZAudioSource> audioSource = std::make_shared<ZALAudioSource>(resource);
    audioSource->playOnLoad_ = playOnLoad;
    if (!playOnLoad) audioSource->Initialize();
    allSamples_.push_front(audioSource);
    return audioSource;
}

bool ZALAudio::ReleaseAudioSource(std::shared_ptr<ZAudioSource> audioSource)
{
    audioSource->Stop();
    allSamples_.remove(audioSource);
    return true;
}

void ZALAudio::CleanUp()
{
    ZAudio::CleanUp();
    // TODO: Might need to clean up sound sources here
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context_);
    alcCloseDevice(device_);
}
