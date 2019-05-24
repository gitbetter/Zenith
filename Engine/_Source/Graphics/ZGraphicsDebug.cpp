/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZGraphicsDebug.cpp

	Created by Adrian Sanchez on 24/05/2019.
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

#include "ZGraphicsDebug.hpp"
#include "ZShader.hpp"
#include "ZGame.hpp"

void ZGraphicsDebug::Initialize() {
	shader_ = std::unique_ptr<ZShader>(new ZShader("Assets/Shaders/Vertex/debug.vert", "Assets/Shaders/Pixel/debug.frag"));
	shader_->Initialize();
}

void ZGraphicsDebug::DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color) {
	if (!zenith::Game()->ActiveScene()) return;

	glm::mat4 VPMatrix = zenith::Game()->ActiveScene()->ViewProjection();

	shader_->Activate();
	shader_->SetMat4("ViewProjection", VPMatrix);
	shader_->SetVec4("color", color);

	std::vector<ZVertex3D> vertices({
	  ZVertex3D(from), ZVertex3D(to)
    });

	ZBufferData bufferData = zenith::Graphics()->Strategy()->LoadVertexData(vertices);
	zenith::Graphics()->Strategy()->DrawLines(bufferData, vertices);
	zenith::Graphics()->Strategy()->DeleteBufferData(bufferData);
}
