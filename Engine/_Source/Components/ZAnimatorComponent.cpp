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

ZIDSequence ZAnimatorComponent::idGenerator_;

ZAnimatorComponent::ZAnimatorComponent()
{
    currentClip.state = ZAnimationState::Invalid;
    name = "AnimatorComponent" + std::to_string(idGenerator_.Next());
}

void ZAnimatorComponent::OnUpdate(double deltaTime)
{
    if (ZServices::GameObjectManager()->Scene(rootObject)->PlayState() != ZPlayState::Playing)
        return;

    if (currentClip.state == ZAnimationState::Playing || currentClip.state == ZAnimationState::Looping)
    {
        currentClip.currentTime += deltaTime;
        if (currentClip.startTime + currentClip.currentTime <= currentClip.endTime)
        {
            ZServices::ModelManager()->BoneTransform(currentClip.model, currentClip.name, currentClip.currentTime);
        }
        else if (currentClip.state == ZAnimationState::Looping)
        {
            double duration = currentClip.endTime - currentClip.startTime;
            currentClip.startTime = SECONDS_TIME;
            currentClip.endTime = currentClip.startTime + duration;
            currentClip.currentTime = 0.0;
        }
        else
        {
            currentClip.state = ZAnimationState::Stopped;
        }
    }
}

void ZAnimatorComponent::OnCloned(const ZHComponent& original)
{
    ZAnimatorComponent* originalObj = ZServices::ComponentManager()->Dereference<ZAnimatorComponent>(original);
    currentClip = originalObj->currentClip;
}

void ZAnimatorComponent::Play(const std::string& animationName, bool looping)
{
    ZHComponent graphics = ZServices::GameObjectManager()->FindComponent<ZGraphicsComponent>(rootObject);
    if (graphics.IsNull())
    {
        LOG("Could not play animator animation. No graphics component exists for this game object.", ZSeverity::Error);
        return;
    }

    ZGraphicsComponent* graphicsObj = ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(graphics);

    if (currentClip.state == ZAnimationState::Paused && !currentClip.model.IsNull())
    {
        currentClip.state = looping ? ZAnimationState::Looping : ZAnimationState::Playing;
    }
    else
    {
        // TODO: Update animator to support multiple models on graphics component
        // multiple simultaneous clips on separate models with the same animation name might be the way to go
        for (const ZHModel& model : graphicsObj->models)
        {
            if (!model.IsNull())
            {
                currentClip.model = model;
                auto animations = ZServices::ModelManager()->Animations(currentClip.model);
                const ZAnimation& animation = animations[animationName];
                currentClip.name = animationName;
                currentClip.currentTime = 0.0;
                currentClip.startTime = SECONDS_TIME;
                currentClip.endTime = currentClip.startTime + animation.duration;
                currentClip.state = looping ? ZAnimationState::Looping : ZAnimationState::Playing;
                break;
            }
        }
    }
}

void ZAnimatorComponent::Pause()
{
    if (currentClip.state == ZAnimationState::Playing)
    {
        currentClip.state = ZAnimationState::Paused;
    }
}

void ZAnimatorComponent::Stop()
{
    if (currentClip.state == ZAnimationState::Playing)
    {
        currentClip.state = ZAnimationState::Stopped;
    }
}

RTTR_REGISTRATION
{
	using namespace rttr;
    registration::class_<ZAnimatorComponent>("ZAnimatorComponent").constructor<>();
}