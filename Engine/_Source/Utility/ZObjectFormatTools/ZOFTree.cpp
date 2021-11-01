/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZOFTree.cpp

    Created by Adrian Sanchez on 08/22/2021.
    Copyright © 2021 Pervasive Sense. All rights reserved.

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

#include "ZOFTree.hpp"

ZOFObjectType ZOFObjectNode::StringToType(const std::string& type)
{
	if (type == "Config")
	{
		return ZOFObjectType::Config;
	}
	if (type == "Theme")
	{
		return ZOFObjectType::Theme;
	}
	if (type == "GameObject")
	{
		return ZOFObjectType::GameObject;
	}
	if (type == "Light")
	{
		return ZOFObjectType::Light;
	}
	if (type == "Camera")
	{
		return ZOFObjectType::Camera;
	}
	if (type == "Grass")
	{
		return ZOFObjectType::Grass;
	}
	if (type == "Material")
	{
		return ZOFObjectType::Material;
	}
	if (type == "Texture")
	{
		return ZOFObjectType::Texture;
	}
	if (type == "Shader")
	{
		return ZOFObjectType::Shader;
	}
	if (type == "Model")
	{
		return ZOFObjectType::Model;
	}
	if (type == "Scene")
	{
		return ZOFObjectType::Scene;
	}
	if (type == "Skybox")
	{
		return ZOFObjectType::Skybox;
	}
	if (type == "Script")
	{
		return ZOFObjectType::Script;
	}
	if (type == "GraphicsComponent")
	{
		return ZOFObjectType::GraphicsComponent;
	}
	if (type == "PhysicsComponent")
	{
		return ZOFObjectType::PhysicsComponent;
	}
	if (type == "ColliderComponent")
	{
		return ZOFObjectType::ColliderComponent;
	}
	if (type == "AnimatorComponent")
	{
		return ZOFObjectType::AnimatorComponent;
	}
	return ZOFObjectType::Any;
}
