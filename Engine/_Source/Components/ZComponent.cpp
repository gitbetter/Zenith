/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

  ZComponent.cpp

  Created by Adrian Sanchez on 28/01/2019.
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
#include "ZGraphicsComponent.hpp"
#include "ZCameraComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZAnimatorComponent.hpp"
#include "ZGameObject.hpp"

std::map<std::string, ZComponent::Creator> ZComponent::componentCreators_ = {
    { "GraphicsComponent", &ZComponent::CreateGraphicsComponent },
    { "CameraComponent", &ZComponent::CreateCameraComponent },
    { "PhysicsComponent", &ZComponent::CreatePhysicsComponent },
    { "AnimatorComponent", &ZComponent::CreateAnimatorComponent }
};


std::shared_ptr<ZComponent> ZComponent::CreateGraphicsComponent(const std::shared_ptr<ZGameObject>& gameObject)
{
    std::shared_ptr<ZGraphicsComponent> comp(new ZGraphicsComponent);
    gameObject->AddComponent(comp);
    return comp;
}

std::shared_ptr<ZComponent> ZComponent::CreateCameraComponent(const std::shared_ptr<ZGameObject>& gameObject)
{
    std::shared_ptr<ZCameraComponent> comp(new ZCameraComponent);
    gameObject->AddComponent(comp);
    return comp;
}

std::shared_ptr<ZComponent> ZComponent::CreatePhysicsComponent(const std::shared_ptr<ZGameObject>& gameObject)
{
    std::shared_ptr<ZPhysicsComponent> comp(new ZPhysicsComponent);
    gameObject->AddComponent(comp);
    return comp;
}

std::shared_ptr<ZComponent> ZComponent::CreateAnimatorComponent(const std::shared_ptr<ZGameObject>& gameObject)
{
    std::shared_ptr<ZAnimatorComponent> comp(new ZAnimatorComponent);
    gameObject->AddComponent(comp);
    return comp;
}

void ZComponent::CreateIn(const std::string& type, const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFNode>& data)
{
    if (componentCreators_.find(type) != componentCreators_.end())
    {
        std::shared_ptr<ZComponent> comp = (componentCreators_[type])(gameObject);
        if (data) {
            comp->Initialize(data);
        }
        else {
            comp->Initialize();
        }
    }
    else
    {
        zenith::Log("Component " + type + " is not available for creation", ZSeverity::Warning);
    }
}
