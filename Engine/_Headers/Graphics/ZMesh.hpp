/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZMesh.hpp

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

#pragma once

#include "ZCommon.hpp"
#include "ZVertexBuffer.hpp"

class ZMaterial;
class ZRenderStateGroup;

struct ZMesh
{

    ZMesh() = default;
    virtual ~ZMesh() = default;

    virtual void Initialize() = 0;

    ZVertexBuffer::ptr bufferData = nullptr;
    std::shared_ptr<ZRenderStateGroup> renderState = nullptr;
    std::string name;

protected:

    static ZIDSequence idGenerator_;

};

struct ZMesh2D : public ZMesh
{

    ZMesh2D(const ZVertex2DDataOptions& vertexData)
        : ZMesh(), vertexData_(vertexData)
    { }
    ~ZMesh2D() = default;

    void Initialize() override;

    ZVertex2DList Vertices() const { return vertexData_.vertices; }
    bool Instanced() const { return vertexData_.instanced.count > 1; }

protected:

    ZVertex2DDataOptions vertexData_;

};

struct ZMesh3D : public ZMesh
{

    friend class ZModelImporter;

    ZMesh3D(const ZVertex3DDataOptions& vertexData);
    ~ZMesh3D();

    void Initialize() override;

    const ZInstancedDataOptions& InstanceData() const { return vertexData_.instanced; }
    const ZVertex3DList& Vertices() const { return vertexData_.vertices; }
    const std::vector<unsigned int>& Indices() const { return vertexData_.indices; }
    bool Instanced() const { return vertexData_.instanced.count > 1; }

    void SetInstanceData(const ZInstancedDataOptions& data);
    void SetVertices(const ZVertex3DList& data);
    void SetIndices(const std::vector<unsigned int>& data);

private:

    ZVertex3DDataOptions vertexData_;

};

using ZMesh3DList = std::vector<ZMesh3D>;