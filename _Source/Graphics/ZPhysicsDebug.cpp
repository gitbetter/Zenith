/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZPhysicsDebug.cpp

	Created by Adrian Sanchez on 26/02/2019.
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

#include "ZPhysicsDebug.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZCameraComponent.hpp"
#include "ZGame.hpp"

void ZPhysicsDebug::Initialize() {
	shader_ = std::unique_ptr<ZShader>(new ZShader("Assets/Shaders/Vertex/debug.vert", "Assets/Shaders/Pixel/debug.frag"));
	shader_->Initialize();
}

void ZPhysicsDebug::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
	if (!ZEngine::Game()->ActiveScene()) return;

	glm::mat4 VPMatrix = ZEngine::Game()->ActiveScene()->ViewProjection();

	shader_->Activate();
	shader_->SetMat4("ViewProjection", VPMatrix);
	shader_->SetVec4("color", glm::vec4(color.x(), color.y(), color.z(), 1.f));

	std::vector<ZVertex3D> vertices({
	  ZVertex3D(glm::vec3(from.x(), from.y(), from.z())),
	  ZVertex3D(glm::vec3(to.x(), to.y(), to.z()))
		});

	ZBufferData bufferData = ZEngine::Graphics()->Strategy()->LoadVertexData(vertices);
	ZEngine::Graphics()->Strategy()->DrawLines(bufferData, vertices);
	ZEngine::Graphics()->Strategy()->DeleteBufferData(bufferData);
}
