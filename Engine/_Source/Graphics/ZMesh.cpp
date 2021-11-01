/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZMesh.cpp

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

#include "ZMesh.hpp"
#include "ZServices.hpp"
#include "ZMaterial.hpp"
#include "ZRenderStateGroup.hpp"

ZIDSequence ZMesh::idGenerator_;

/****************** ZMesh2D ******************/

void ZMesh2D::Initialize()
{
    bufferData = ZVertexBuffer::Create(vertexData_);

    ZRenderStateGroupWriter writer;
    writer.Begin();
	writer.BindVertexBuffer(bufferData);
    renderState = writer.End();
}

std::shared_ptr<ZMesh2D> ZMesh2D::NewQuad()
{
    ZVertex2DDataOptions options;
    options.vertices = ZVertex2DList{
        ZVertex2D(-1.f, 1.f, 0.f, 1.f),
        ZVertex2D(-1.f, -1.f, 0.f, 0.f),
        ZVertex2D(1.f, -1.f, 1.f, 0.f),
        ZVertex2D(1.f, 1.f, 1.f, 1.f)
    };
    std::shared_ptr<ZMesh2D> mesh = std::make_shared<ZMesh2D>(options);
    mesh->Initialize();
    return mesh;
}

std::shared_ptr<ZMesh2D> ZMesh2D::NewScreenTriangle()
{
    ZVertex2DDataOptions options;
    options.vertices = ZVertex2DList{
        ZVertex2D(-1.f, -1.f, 0.f, 0.f),
        ZVertex2D(3.f, -1.f, 2.f, 0.f),
        ZVertex2D(-1.f, 3.f, 0.f, 2.f)
    };
    std::shared_ptr<ZMesh2D> mesh = std::make_shared<ZMesh2D>(options);
    mesh->Initialize();
    return mesh;
}

/****************** ZMesh3D ******************/

ZMesh3D::ZMesh3D(const ZVertex3DDataOptions& vertexData)
    : ZMesh(), vertexData_(vertexData)
{
    name = "ZMSH3D_" + std::to_string(idGenerator_.Next());
}

ZMesh3D::~ZMesh3D()
{
    vertexData_.vertices.clear();
    vertexData_.indices.clear();
}

void ZMesh3D::Initialize()
{
    bufferData = ZVertexBuffer::Create(vertexData_);

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.BindVertexBuffer(bufferData);
    renderState = writer.End();
}

void ZMesh3D::SetInstanceData(const ZInstancedDataOptions& data)
{
    vertexData_.instanced = data;
    bufferData->Update(vertexData_);
}

void ZMesh3D::SetVertices(const ZVertex3DList& vertices)
{
    vertexData_.vertices = vertices;
    bufferData->Update(vertexData_);
}

void ZMesh3D::SetIndices(const std::vector<unsigned int>& indices)
{
    vertexData_.indices = indices;
    bufferData->Update(vertexData_);
}