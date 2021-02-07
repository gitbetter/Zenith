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

#include "ZServices.hpp"
#include "ZMesh2D.hpp"
#include "ZShader.hpp"

void ZMesh2D::Initialize()
{
    bufferData_ = ZBuffer::Create(vertexData_);
}

void ZMesh2D::Render(const std::shared_ptr<ZShader>& shader, const std::shared_ptr<ZMaterial>& material)
{
    if (material)
    {
        shader->Use(material);
    }
    if (vertexData_.instanced.count > 1)
    {
        shader->SetBool("instanced", true);
    }
    ZServices::Graphics()->Draw(bufferData_, vertexData_, drawStyle_);
}

std::shared_ptr<ZMesh2D> ZMesh2D::NewQuad()
{
    ZVertex2DDataOptions options;
    options.vertices = std::vector<ZVertex2D>{
        ZVertex2D(-1.f, 1.f, 0.f, 1.f),
        ZVertex2D(-1.f, -1.f, 0.f, 0.f),
        ZVertex2D(1.f, -1.f, 1.f, 0.f),
        ZVertex2D(1.f, 1.f, 1.f, 1.f)
    };
    std::shared_ptr<ZMesh2D> mesh = std::make_shared<ZMesh2D>(options, ZMeshDrawStyle::TriangleFan);
    mesh->Initialize();
    return mesh;
}

std::shared_ptr<ZMesh2D> ZMesh2D::NewScreenTriangle()
{
    ZVertex2DDataOptions options;
    options.vertices = std::vector<ZVertex2D>{
        ZVertex2D(-1.f, -1.f, 0.f, 0.f),
        ZVertex2D(3.f, -1.f, 2.f, 0.f),
        ZVertex2D(-1.f, 3.f, 0.f, 2.f)
    };
    std::shared_ptr<ZMesh2D> mesh = std::make_shared<ZMesh2D>(options);
    mesh->Initialize();
    return mesh;
}
