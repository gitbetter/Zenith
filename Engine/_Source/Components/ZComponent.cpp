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
#include "ZScriptComponent.hpp"
#include "ZGameObject.hpp"
#include "ZOFTree.hpp"
#include "ZServices.hpp"

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace rttr;
    registration::class_<ZComponent>("ZComponent");
}

bool ZComponent::MultipleSupported(ZComponentType type)
{
	switch (type)
	{
	case ZComponentType::Graphics:
	case ZComponentType::Physics:
	case ZComponentType::Animator:
		return false;
		break;
	case ZComponentType::Collider:
	case ZComponentType::Script:
	case ZComponentType::Other:
		return true;
		break;
	default:
		break;
	}
	return false;
}

ZComponentType ZComponent::StringToComponentType(const std::string& type)
{
	if (type == "Script")
	{
		return ZComponentType::Script;
	}
	else if (type == "Physics")
	{
		return ZComponentType::Physics;
	}
	else if (type == "Graphics")
	{
		return ZComponentType::Graphics;
	}
	else if (type == "Collider")
	{
		return ZComponentType::Collider;
	}
	else if (type == "Animator")
	{
		return ZComponentType::Animator;
	}
	return ZComponentType::Other;
}

ZHComponent ZComponentManager::CreateIn(ZComponentType type, const ZHGameObject& gameObject, const std::shared_ptr<ZOFObjectNode>& data /*= nullptr*/)
{
    ZHComponent handle;
    ZComponent* comp = nullptr;

    switch (type)
    {
    case ZComponentType::Graphics:
        comp = resourcePool_.New<ZGraphicsComponent>(handle);
        break;
    case ZComponentType::Physics:
        comp = resourcePool_.New<ZPhysicsComponent>(handle);
        break;
    case ZComponentType::Animator:
        comp = resourcePool_.New<ZAnimatorComponent>(handle);
        break;
    case ZComponentType::Collider:
        comp = resourcePool_.New<ZColliderComponent>(handle);
        break;
    case ZComponentType::Script:
        comp = resourcePool_.New<ZScriptComponent>(handle);
        break;
    case ZComponentType::Other:
        break;
    default:
        break;
    }

    if (!handle.IsNull())
    {
	    ZServices::GameObjectManager()->AddComponent(gameObject, handle);
        comp->OnCreate();
	    if (data)
	    {
		    comp->OnDeserialize(data);
	    }
    }
	return handle;
}