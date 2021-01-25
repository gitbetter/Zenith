/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZMesh2D.cpp

    Created by Adrian Sanchez on 06/02/2019.
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

#include "ZUI.hpp"
#include "ZMesh2D.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZShader.hpp"

void ZMesh2D::Initialize()
{
    bufferData_ = zenith::Graphics()->LoadVertexData(vertexData_);
}

void ZMesh2D::Render(ZShader* shader, ZMaterial* material)
{
    if (material)
    {
        shader->Use(material);
    }
    if (vertexData_.instanced.count > 1)
    {
        shader->SetBool("instanced", true);
    }
    zenith::Graphics()->Draw(bufferData_, vertexData_);
}

std::shared_ptr<ZMesh2D> ZMesh2D::NewQuad()
{
    ZVertex2DDataOptions options;
    options.vertices = std::vector<ZVertex2D>{
        ZVertex2D(glm::vec2(-1.f, 1.f), glm::vec2(0.f)),
        ZVertex2D(glm::vec2(-1.f, -1.f), glm::vec2(0.f, 1.f)),
        ZVertex2D(glm::vec2(1.f, 1.f), glm::vec2(1.f, 0.f)),
        ZVertex2D(glm::vec2(1.f, -1.f), glm::vec2(1.f))
    };
    std::shared_ptr<ZMesh2D> mesh = std::make_shared<ZMesh2D>(options);
    mesh->Initialize();
    return mesh;
}
