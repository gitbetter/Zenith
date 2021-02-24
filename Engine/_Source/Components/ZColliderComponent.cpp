/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

  ZColliderComponent.cpp

  Created by Adrian Sanchez on 02/20/2021.
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

#include "ZColliderComponent.hpp"
#include "ZServices.hpp"
#include "ZCollider.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsComponent.hpp"

ZColliderComponent::ZColliderComponent()
{
    id_ = "ZCOMP_COLLIDER_" + idGenerator_.Next();
}

void ZColliderComponent::Initialize(ZColliderType type, const glm::vec3& extents, const glm::vec3& offset)
{
    collider_ = ZCollider::Create(type, extents, offset);
}

void ZColliderComponent::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZColliderComponent", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    std::string shape;
    if (props.find("shape") != props.end() && props["shape"]->HasValues())
    {
        std::shared_ptr<ZOFString> shapeProp = props["shape"]->Value<ZOFString>(0);
        shape = shapeProp->value;
    }

    glm::vec3 extents(0.f);
    if (props.find("extents") != props.end() && props["extents"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> extentsProp = props["extents"]->Value<ZOFNumberList>(0);
        extents = glm::vec3(extentsProp->value[0], extentsProp->value[1], extentsProp->value[2]);
    }

    glm::vec3 offset(0.f);
    if (props.find("offset") != props.end() && props["offset"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> offsetProp = props["offset"]->Value<ZOFNumberList>(0);
        offset = glm::vec3(offsetProp->value[0], offsetProp->value[1], offsetProp->value[2]);
    }

    ZColliderType type = ZColliderType::None;
    if (shape == "Box") {
        type = ZColliderType::Box;
    }
    else if (shape == "Sphere") {
        type = ZColliderType::Sphere;
    }
    else if (shape == "Capsule") {
        type = ZColliderType::Capsule;
    }
    else if (shape == "Cylinder") {
        type = ZColliderType::Cylinder;
    }
    else if (shape == "Cone") {
        type = ZColliderType::Cone;
    }
    Initialize(type, extents, offset);
}

void ZColliderComponent::Update(double deltaTime)
{
    AddColliderIfNecessary();
}

std::shared_ptr<ZComponent> ZColliderComponent::Clone()
{
    auto comp = std::make_shared<ZColliderComponent>();
    return comp;
}

void ZColliderComponent::AddColliderIfNecessary()
{
    if (!added_) {
        if (auto physicsComp = object_->FindComponent<ZPhysicsComponent>()) {
            physicsComp->AddCollider(collider_);
            added_ = true;
        }
    }
}

DEFINE_COMPONENT_CREATORS(ZColliderComponent)
