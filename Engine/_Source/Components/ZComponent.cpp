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
#include "ZPhysicsComponent.hpp"
#include "ZAnimatorComponent.hpp"
#include "ZColliderComponent.hpp"
#include "ZGameObject.hpp"
#include "ZServices.hpp"

#include <rttr/registration>

ZIDSequence ZComponent::idGenerator_;

std::map<ZOFObjectType, ZComponent::Creator> ZComponent::componentCreators_ = {
    { ZOFObjectType::GraphicsComponent, &ZComponent::CreateGraphicsComponent },
    { ZOFObjectType::PhysicsComponent, &ZComponent::CreatePhysicsComponent },
    { ZOFObjectType::AnimatorComponent, &ZComponent::CreateAnimatorComponent },
    { ZOFObjectType::ColliderComponent, &ZComponent::CreateColliderComponent },
};


std::shared_ptr<ZComponent> ZComponent::CreateGraphicsComponent(const ZHGameObject& gameObject, const std::shared_ptr<ZOFObjectNode>& data)
{
    return ZGraphicsComponent::CreateIn(gameObject, data);
}

std::shared_ptr<ZComponent> ZComponent::CreatePhysicsComponent(const ZHGameObject& gameObject, const std::shared_ptr<ZOFObjectNode>& data)
{
    return ZPhysicsComponent::CreateIn(gameObject, data);
}

std::shared_ptr<ZComponent> ZComponent::CreateAnimatorComponent(const ZHGameObject& gameObject, const std::shared_ptr<ZOFObjectNode>& data)
{
    return ZAnimatorComponent::CreateIn(gameObject, data);
}

std::shared_ptr<ZComponent> ZComponent::CreateColliderComponent(const ZHGameObject& gameObject, const std::shared_ptr<ZOFObjectNode>& data)
{
    return ZColliderComponent::CreateIn(gameObject, data);
}

void ZComponent::CreateIn(const ZHGameObject& gameObject, const std::shared_ptr<ZOFObjectNode>& data)
{
    using namespace zenith::strings;
    if (componentCreators_.find(data->type) != componentCreators_.end())
    {
        std::shared_ptr<ZComponent> comp = (componentCreators_[data->type])(gameObject, data);
    }
    else
    {
        LOG("Component type is not available for creation", ZSeverity::Warning);
    }
}

RTTR_REGISTRATION
{
	using namespace rttr;
    registration::class_<ZComponent>("ZComponent");
}