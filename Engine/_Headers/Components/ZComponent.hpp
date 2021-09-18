/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZComponent.hpp

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

#pragma once

#define DECLARE_COMPONENT_CREATORS(Type)\
static std::shared_ptr<Type> Create();\
static std::shared_ptr<Type> CreateIn(const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFNode>& data = nullptr);

#define DEFINE_COMPONENT_CREATORS(Type)\
std::shared_ptr<Type> Type::Create()\
{\
    std::shared_ptr<Type> comp = std::make_shared<Type>();\
    return comp;\
}\
std::shared_ptr<Type> Type::CreateIn(const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFNode>& data)\
{\
    std::shared_ptr<Type> comp = std::make_shared<Type>();\
    gameObject->AddComponent(comp);\
    if (data) {\
        comp->Initialize(data);\
    }\
    return comp;\
}

#include "ZIDSequence.hpp"
#include "ZProcess.hpp"
#include <rttr/type>

class ZComponent : public ZProcess
{
    RTTR_ENABLE()

    friend class ZGameObject;

    using Creator = std::shared_ptr<ZComponent>(*)(const std::shared_ptr<class ZGameObject>&, const std::shared_ptr<struct ZOFObjectNode>&);

public:

    virtual ~ZComponent() {}

    virtual void Initialize() override { ZProcess::Initialize(); }
    virtual void Initialize(std::shared_ptr<ZOFNode> root) = 0;

    virtual void CleanUp() { Abort(); }

    virtual std::shared_ptr<ZComponent> Clone() = 0;

    ZGameObject* Object() { return object_; }

    static std::shared_ptr<ZComponent> CreateGraphicsComponent(const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFObjectNode>& data = nullptr);
    static std::shared_ptr<ZComponent> CreatePhysicsComponent(const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFObjectNode>& data = nullptr);
    static std::shared_ptr<ZComponent> CreateAnimatorComponent(const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFObjectNode>& data = nullptr);
    static std::shared_ptr<ZComponent> CreateColliderComponent(const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFObjectNode>& data = nullptr);

    static void CreateIn(const std::shared_ptr<ZGameObject>& gameObject, const std::shared_ptr<ZOFObjectNode>& data = nullptr);

protected:

    ZGameObject* object_;

    static ZIDSequence idGenerator_;
    static std::map<ZOFObjectType, Creator> componentCreators_;

};