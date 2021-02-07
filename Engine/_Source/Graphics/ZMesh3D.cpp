/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZMesh3D.cpp

    Created by Adrian Sanchez on 1/25/19.
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
#include "ZMesh3D.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZSkeleton.hpp"

ZMesh3D::ZMesh3D(const ZVertex3DDataOptions& vertexData, ZMeshDrawStyle drawStyle)
    : vertexData_(vertexData)
{
    drawStyle_ = drawStyle;
    id_ = "ZMSH3D_" + idGenerator_.Next();
}

ZMesh3D::~ZMesh3D()
{
    vertexData_.vertices.clear();
    vertexData_.indices.clear();
}

void ZMesh3D::Initialize()
{
    bufferData_ = ZBuffer::Create(vertexData_);
}

void ZMesh3D::Render(const std::shared_ptr<ZShader>& shader, const std::shared_ptr<ZMaterial>& material)
{
    shader->Use(material);
    auto materialProps = material->Properties();
    if (materialProps.tiling > 1)
    {
        for (auto it = vertexData_.vertices.begin(); it != vertexData_.vertices.end(); it++)
        {
            it->uv *= materialProps.tiling;
        }
        materialProps.tiling = 1.f;
        material->SetProperties(materialProps);
        bufferData_->Update(vertexData_);
    }
    shader->SetBool("instanced", vertexData_.instanced.count > 1);
    ZServices::Graphics()->Draw(bufferData_, vertexData_, drawStyle_);
}

void ZMesh3D::SetInstanceData(const ZInstancedDataOptions& data)
{
    vertexData_.instanced = data;
    bufferData_->Update(vertexData_);
}

void ZMesh3D::SetVertices(const std::vector<ZVertex3D>& vertices)
{
    vertexData_.vertices = vertices;
    bufferData_->Update(vertexData_);
}

void ZMesh3D::SetIndices(const std::vector<unsigned int>& indices)
{
    vertexData_.indices = indices;
    bufferData_->Update(vertexData_);
}