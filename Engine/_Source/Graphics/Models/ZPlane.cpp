/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZPlane.hpp

    Created by Adrian Sanchez on 02/19/21.
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

#include "ZPlane.hpp"
#include "ZServices.hpp"

void ZPlane::Initialize()
{
    float textureTiling = 1.f;

    ZVertex3D topLeft(glm::vec3(-size_.x, 0.f, -size_.y)); topLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D bottomLeft(glm::vec3(-size_.x, 0.f, size_.y)); bottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D bottomRight(glm::vec3(size_.x, 0.f, size_.y)); bottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D topRight(glm::vec3(size_.x, 0.f, -size_.y)); topRight.uv = glm::vec2(textureTiling, textureTiling);
    ZVertex3D::ComputeTangentBitangent(bottomLeft, bottomRight, topRight);
    ZVertex3D::ComputeTangentBitangent(bottomLeft, topRight, topLeft);

    ZVertex3DDataOptions options;
    options.vertices = ZVertex3DList{
        topLeft, bottomLeft, bottomRight, topRight
    };
    options.indices = std::vector<unsigned int>{
        0, 1, 2,
        0, 2, 3
    };


    static std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(options);
    mesh->Initialize();
    meshes_[mesh->ID()] = mesh;

    ZModel::Initialize();
}

void ZPlane::Initialize(const std::shared_ptr<ZOFNode>& data)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(data);
    if (!node)
    {
        LOG("Could not initalize ZPlane: node data is invalid", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("size") != props.end() && props["size"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> lengthProp = props["size"]->Value<ZOFNumberList>(0);
        size_ = glm::vec2(lengthProp->value[0], lengthProp->value[1]);
    }

    Initialize();
}

std::shared_ptr<ZPlane> ZPlane::Create(const glm::vec2& size)
{
    auto plane = std::make_shared<ZPlane>(size);
    plane->Initialize();
    return plane;
}