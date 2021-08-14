/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZAnimatorComponent.cpp

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

#include "ZAnimatorComponent.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZGameObject.hpp"
#include "ZModel.hpp"
#include "ZMesh.hpp"
#include "ZServices.hpp"
#include "ZScene.hpp"

#include <rttr/registration>

ZAnimatorComponent::ZAnimatorComponent()
{
    currentClip_.state = ZAnimationState::Invalid;
    id_ = "ZCOMP_ANIMATOR_" + std::to_string(idGenerator_.Next());
}

void ZAnimatorComponent::Initialize(std::shared_ptr<ZOFNode> root)
{
// TODO: Load from zof tree as usual
}

void ZAnimatorComponent::Update(double deltaTime)
{
    if (object_->Scene()->PlayState() != ZPlayState::Playing)
        return;

    if (currentClip_.state == ZAnimationState::Playing || currentClip_.state == ZAnimationState::Looping)
    {
        currentClip_.currentTime += deltaTime;
        if (currentClip_.startTime + currentClip_.currentTime <= currentClip_.endTime)
        {
            currentClip_.model->BoneTransform(currentClip_.name, currentClip_.currentTime);
        }
        else if (currentClip_.state == ZAnimationState::Looping)
        {
            double duration = currentClip_.endTime - currentClip_.startTime;
            currentClip_.startTime = SECONDS_TIME;
            currentClip_.endTime = currentClip_.startTime + duration;
            currentClip_.currentTime = 0.0;
        }
        else
        {
            currentClip_.state = ZAnimationState::Stopped;
        }
    }
}

std::shared_ptr<ZComponent> ZAnimatorComponent::Clone()
{
    std::shared_ptr<ZAnimatorComponent> clone = std::make_shared<ZAnimatorComponent>();
    clone->currentClip_ = currentClip_;
    return clone;
}

void ZAnimatorComponent::Play(const std::string& animationName, bool looping)
{
    std::shared_ptr<ZGraphicsComponent> graphics = object_->FindComponent<ZGraphicsComponent>();
    if (!graphics)
    {
        LOG("Could not play animator animation. No graphics component exists for this game object.", ZSeverity::Error);
        return;
    }

    if (currentClip_.state == ZAnimationState::Paused && currentClip_.model != nullptr)
    {
        currentClip_.state = looping ? ZAnimationState::Looping : ZAnimationState::Playing;
    }
    else
    {
        currentClip_.model = graphics->Model();

        if (currentClip_.model)
        {
            auto animations = currentClip_.model->Animations();
            std::shared_ptr<ZAnimation> animation = animations[animationName];
            currentClip_.name = animationName;
            currentClip_.currentTime = 0.0;
            currentClip_.startTime = SECONDS_TIME;
            currentClip_.endTime = currentClip_.startTime + animation->duration;
            currentClip_.state = looping ? ZAnimationState::Looping : ZAnimationState::Playing;
        }
    }
}

void ZAnimatorComponent::Pause()
{
    if (currentClip_.state == ZAnimationState::Playing)
        currentClip_.state = ZAnimationState::Paused;
}

void ZAnimatorComponent::Stop()
{
    if (currentClip_.state == ZAnimationState::Playing)
        currentClip_.state = ZAnimationState::Stopped;
}

DEFINE_COMPONENT_CREATORS(ZAnimatorComponent)

RTTR_REGISTRATION
{
	using namespace rttr;
    registration::class_<ZAnimatorComponent>("ZAnimatorComponent").constructor<>();
}