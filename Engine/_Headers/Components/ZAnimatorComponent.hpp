/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZAnimatorComponent.hpp

    Created by Adrian Sanchez on 30/03/2019.
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
#include "ZComponent.hpp"
#include "ZAnimation.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZAnimatorComponent : public ZComponent
{

public:

    ZAnimatorComponent();
    ~ZAnimatorComponent() {}

    void Initialize(std::shared_ptr<ZOFNode> root) override;
    void Update(double deltaTime) override;

    void Play(const std::string& animationName, bool looping = false);
    void Pause() override;
    void Stop();

    std::shared_ptr<ZComponent> Clone() override;

    DECLARE_COMPONENT_CREATORS(ZAnimatorComponent)

protected:

    ZAnimationClip currentClip_;

};
