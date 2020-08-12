/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZAudioSource.hpp

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
#include "ZProcess.hpp"

// Forward Declarations
class ZResourceHandle;

// Class and Data Structure Definitions
class ZAudioSource : public ZProcess
{

    friend class ZALAudio;

public:

    ZAudioSource(std::shared_ptr<ZResourceHandle> resource = nullptr);
    virtual ~ZAudioSource() {}

    virtual std::shared_ptr<ZResourceHandle> const Resource() { return resourceHandle_; }

    virtual void Play(int volume, bool looping = true) = 0;
    virtual void Stop() = 0;

    virtual void TogglePause() = 0;
    virtual bool IsPlaying() = 0;
    virtual bool IsLooping() { return isLooping_; }
    virtual int Volume() const { return volume_; }
    virtual float Progress() const = 0;
    virtual void SetLooping(bool looping) { isLooping_ = looping; }
    virtual void SetVolume(int volume) = 0;

protected:

    std::shared_ptr<ZResourceHandle> resourceHandle_;
    bool isPaused_, isLooping_, playOnLoad_;
    int volume_;

};
