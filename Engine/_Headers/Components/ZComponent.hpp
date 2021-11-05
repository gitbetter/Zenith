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

#include "ZResourceManager.hpp"
#include "ZIDSequence.hpp"
#include "ZOFTree.hpp"
#include <rttr/type>

enum class ZComponentType
{
    Graphics, Physics, Animator, Collider, Script, Other
};

struct ZComponent
{
    RTTR_ENABLE()

public:

    virtual ~ZComponent() = default;

	virtual void OnCreate() { }
	virtual void OnDeserialize(const std::shared_ptr<struct ZOFObjectNode>& dataNode) { }
    virtual void OnUpdate(double deltaTime) { };
    virtual void OnCloned(const ZHComponent& original) { };
    virtual void OnCleanUp() { };

    ZComponentType type;
    ZHGameObject rootObject;
    std::string name;

public:

    static bool MultipleSupported(ZComponentType type);
    static ZComponentType StringToComponentType(const std::string& type);

};

class ZComponentManager : public ZResourceManager<ZComponent, ZHComponent>
{

public:

	virtual ~ZComponentManager() = default;

public:

	virtual void Initialize() override { }
	virtual void CleanUp() override { }

public:

    ZHComponent Create(ZComponentType type, const ZHComponent& restoreHandle = ZHComponent());
    ZHComponent CreateIn(ZComponentType type, const ZHGameObject& gameObject, const std::shared_ptr<struct ZOFObjectNode>& data = nullptr, const ZHComponent& restoreHandle = ZHComponent());
    ZHComponent Deserialize(const ZOFHandle& dataHandle, const std::shared_ptr<struct ZOFObjectNode>& dataNode, const std::shared_ptr<ZScene>& scene = nullptr);

	ZHGameObject Object(const ZHComponent& handle);
    std::string Name(const ZHComponent& handle);

	void Update(const ZHComponent& handle, float deltaTime);
	void CleanUp(const ZHComponent& handle);
    ZHComponent Clone(const ZHComponent& handle);

};