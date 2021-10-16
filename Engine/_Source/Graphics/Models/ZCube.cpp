/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZCube.hpp

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

#include "ZCube.hpp"
#include "ZServices.hpp"

void ZCube::OnCreate()
{
    float textureTiling = 1.f;

    // Front face
    ZVertex3D front_BottomLeft(glm::vec3(-size_.x, -size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_BottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D front_BottomRight(glm::vec3(size_.x, -size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_BottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D front_TopRight(glm::vec3(size_.x, size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_TopRight.uv = glm::vec2(textureTiling, textureTiling);
    ZVertex3D front_TopLeft(glm::vec3(-size_.x, size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_TopLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D::ComputeTangentBitangent(front_BottomLeft, front_BottomRight, front_TopRight);
    ZVertex3D::ComputeTangentBitangent(front_BottomLeft, front_TopRight, front_TopLeft);

    // Back face
    ZVertex3D back_BottomLeft(glm::vec3(size_.x, -size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_BottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D back_BottomRight(glm::vec3(-size_.x, -size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_BottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D back_TopRight(glm::vec3(-size_.x, size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_TopRight.uv = glm::vec2(textureTiling, textureTiling);
    ZVertex3D back_TopLeft(glm::vec3(size_.x, size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_TopLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D::ComputeTangentBitangent(back_BottomLeft, back_BottomRight, back_TopRight);
    ZVertex3D::ComputeTangentBitangent(back_BottomLeft, back_TopRight, back_TopLeft);

    // Right Face
    ZVertex3D right_BottomLeft(glm::vec3(size_.x, -size_.y, size_.z), glm::vec3(1.f, 0.f, 0.f)); right_BottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D right_BottomRight(glm::vec3(size_.x, -size_.y, -size_.z), glm::vec3(1.f, 0.f, 0.f)); right_BottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D right_TopRight(glm::vec3(size_.x, size_.y, -size_.z), glm::vec3(1.f, 0.f, 0.f)); right_TopRight.uv = glm::vec2(textureTiling, textureTiling);
    ZVertex3D right_TopLeft(glm::vec3(size_.x, size_.y, size_.z), glm::vec3(1.f, 0.f, 0.f)); right_TopLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D::ComputeTangentBitangent(right_BottomLeft, right_BottomRight, right_TopRight);
    ZVertex3D::ComputeTangentBitangent(right_BottomLeft, right_TopRight, right_TopLeft);

    // Left face
    ZVertex3D left_BottomLeft(glm::vec3(-size_.x, -size_.y, -size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_BottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D left_BottomRight(glm::vec3(-size_.x, -size_.y, size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_BottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D left_TopRight(glm::vec3(-size_.x, size_.y, size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_TopRight.uv = glm::vec2(textureTiling, textureTiling);
    ZVertex3D left_TopLeft(glm::vec3(-size_.x, size_.y, -size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_TopLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D::ComputeTangentBitangent(left_BottomLeft, left_BottomRight, left_TopRight);
    ZVertex3D::ComputeTangentBitangent(left_BottomLeft, left_TopRight, left_TopLeft);

    // Top face
    ZVertex3D top_BottomLeft(glm::vec3(-size_.x, size_.y, size_.z), glm::vec3(0.f, 1.f, 0.f)); top_BottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D top_BottomRight(glm::vec3(size_.x, size_.y, size_.z), glm::vec3(0.f, 1.f, 0.f)); top_BottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D top_TopRight(glm::vec3(size_.x, size_.y, -size_.z), glm::vec3(0.f, 1.f, 0.f)); top_TopRight.uv = glm::vec2(textureTiling, textureTiling);
    ZVertex3D top_TopLeft(glm::vec3(-size_.x, size_.y, -size_.z), glm::vec3(0.f, 1.f, 0.f)); top_TopLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D::ComputeTangentBitangent(top_BottomLeft, top_BottomRight, top_TopRight);
    ZVertex3D::ComputeTangentBitangent(top_BottomLeft, top_TopRight, top_TopLeft);

    // Bottom face
    ZVertex3D bottom_BottomLeft(glm::vec3(-size_.x, -size_.y, size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_BottomLeft.uv = glm::vec2(0.f, 0.f);
    ZVertex3D bottom_BottomRight(glm::vec3(size_.x, -size_.y, size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_BottomRight.uv = glm::vec2(textureTiling, 0.f);
    ZVertex3D bottom_TopRight(glm::vec3(size_.x, -size_.y, -size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_TopRight.uv = glm::vec2(textureTiling, textureTiling);
    ZVertex3D bottom_TopLeft(glm::vec3(-size_.x, -size_.y, -size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_TopLeft.uv = glm::vec2(0.f, textureTiling);
    ZVertex3D::ComputeTangentBitangent(bottom_BottomLeft, bottom_BottomRight, bottom_TopRight);
    ZVertex3D::ComputeTangentBitangent(bottom_BottomLeft, bottom_TopRight, bottom_TopLeft);

    ZVertex3DDataOptions options;
    options.vertices = ZVertex3DList{
        front_BottomLeft, front_BottomRight, front_TopRight, front_TopLeft,
        right_BottomLeft, right_BottomRight, right_TopRight, right_TopLeft,
        back_BottomLeft, back_BottomRight, back_TopRight, back_TopLeft,
        left_BottomLeft, left_BottomRight, left_TopRight, left_TopLeft,
        top_BottomLeft, top_BottomRight, top_TopRight, top_TopLeft,
        bottom_BottomLeft, bottom_BottomRight, bottom_TopRight, bottom_TopLeft
    };
    options.indices = std::vector<unsigned int>{
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    ZMesh3D mesh(options);
    mesh.Initialize();
    meshes.emplace_back(mesh);
}

void ZCube::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (!dataNode)
    {
        LOG("Could not initalize ZPlane: node data is invalid", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("size") != props.end() && props["size"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> lengthProp = props["size"]->Value<ZOFNumberList>(0);
        size_ = glm::vec3(lengthProp->value[0], lengthProp->value[1], lengthProp->value[2]);
    }

	float textureTiling = 1.f;

	// Front face
	ZVertex3D front_BottomLeft(glm::vec3(-size_.x, -size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D front_BottomRight(glm::vec3(size_.x, -size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D front_TopRight(glm::vec3(size_.x, size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D front_TopLeft(glm::vec3(-size_.x, size_.y, size_.z), glm::vec3(0.f, 0.f, 1.f)); front_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZVertex3D::ComputeTangentBitangent(front_BottomLeft, front_BottomRight, front_TopRight);
	ZVertex3D::ComputeTangentBitangent(front_BottomLeft, front_TopRight, front_TopLeft);

	// Back face
	ZVertex3D back_BottomLeft(glm::vec3(size_.x, -size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D back_BottomRight(glm::vec3(-size_.x, -size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D back_TopRight(glm::vec3(-size_.x, size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D back_TopLeft(glm::vec3(size_.x, size_.y, -size_.z), glm::vec3(0.f, 0.f, -1.f)); back_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZVertex3D::ComputeTangentBitangent(back_BottomLeft, back_BottomRight, back_TopRight);
	ZVertex3D::ComputeTangentBitangent(back_BottomLeft, back_TopRight, back_TopLeft);

	// Right Face
	ZVertex3D right_BottomLeft(glm::vec3(size_.x, -size_.y, size_.z), glm::vec3(1.f, 0.f, 0.f)); right_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D right_BottomRight(glm::vec3(size_.x, -size_.y, -size_.z), glm::vec3(1.f, 0.f, 0.f)); right_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D right_TopRight(glm::vec3(size_.x, size_.y, -size_.z), glm::vec3(1.f, 0.f, 0.f)); right_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D right_TopLeft(glm::vec3(size_.x, size_.y, size_.z), glm::vec3(1.f, 0.f, 0.f)); right_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZVertex3D::ComputeTangentBitangent(right_BottomLeft, right_BottomRight, right_TopRight);
	ZVertex3D::ComputeTangentBitangent(right_BottomLeft, right_TopRight, right_TopLeft);

	// Left face
	ZVertex3D left_BottomLeft(glm::vec3(-size_.x, -size_.y, -size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D left_BottomRight(glm::vec3(-size_.x, -size_.y, size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D left_TopRight(glm::vec3(-size_.x, size_.y, size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D left_TopLeft(glm::vec3(-size_.x, size_.y, -size_.z), glm::vec3(-1.f, 0.f, 0.f)); left_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZVertex3D::ComputeTangentBitangent(left_BottomLeft, left_BottomRight, left_TopRight);
	ZVertex3D::ComputeTangentBitangent(left_BottomLeft, left_TopRight, left_TopLeft);

	// Top face
	ZVertex3D top_BottomLeft(glm::vec3(-size_.x, size_.y, size_.z), glm::vec3(0.f, 1.f, 0.f)); top_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D top_BottomRight(glm::vec3(size_.x, size_.y, size_.z), glm::vec3(0.f, 1.f, 0.f)); top_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D top_TopRight(glm::vec3(size_.x, size_.y, -size_.z), glm::vec3(0.f, 1.f, 0.f)); top_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D top_TopLeft(glm::vec3(-size_.x, size_.y, -size_.z), glm::vec3(0.f, 1.f, 0.f)); top_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZVertex3D::ComputeTangentBitangent(top_BottomLeft, top_BottomRight, top_TopRight);
	ZVertex3D::ComputeTangentBitangent(top_BottomLeft, top_TopRight, top_TopLeft);

	// Bottom face
	ZVertex3D bottom_BottomLeft(glm::vec3(-size_.x, -size_.y, size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_BottomLeft.uv = glm::vec2(0.f, 0.f);
	ZVertex3D bottom_BottomRight(glm::vec3(size_.x, -size_.y, size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_BottomRight.uv = glm::vec2(textureTiling, 0.f);
	ZVertex3D bottom_TopRight(glm::vec3(size_.x, -size_.y, -size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_TopRight.uv = glm::vec2(textureTiling, textureTiling);
	ZVertex3D bottom_TopLeft(glm::vec3(-size_.x, -size_.y, -size_.z), glm::vec3(0.f, -1.f, 0.f)); bottom_TopLeft.uv = glm::vec2(0.f, textureTiling);
	ZVertex3D::ComputeTangentBitangent(bottom_BottomLeft, bottom_BottomRight, bottom_TopRight);
	ZVertex3D::ComputeTangentBitangent(bottom_BottomLeft, bottom_TopRight, bottom_TopLeft);

	ZVertex3DDataOptions options;
	options.vertices = ZVertex3DList{
		front_BottomLeft, front_BottomRight, front_TopRight, front_TopLeft,
		right_BottomLeft, right_BottomRight, right_TopRight, right_TopLeft,
		back_BottomLeft, back_BottomRight, back_TopRight, back_TopLeft,
		left_BottomLeft, left_BottomRight, left_TopRight, left_TopLeft,
		top_BottomLeft, top_BottomRight, top_TopRight, top_TopLeft,
		bottom_BottomLeft, bottom_BottomRight, bottom_TopRight, bottom_TopLeft
	};
	options.indices = std::vector<unsigned int>{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	ZMesh3D mesh(options);
	mesh.Initialize();
	meshes.emplace_back(mesh);
}