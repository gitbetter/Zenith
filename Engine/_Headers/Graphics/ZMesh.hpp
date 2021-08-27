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

#include "ZProcess.hpp"
#include "ZVertexBuffer.hpp"

class ZMaterial;
class ZRenderStateGroup;

class ZMesh : public ZProcess
{

public:

    ZMesh() { }
    virtual ~ZMesh() {}

    virtual void Initialize() = 0;

    const ZVertexBuffer::ptr& BufferData() const { return bufferData_; }
    const std::shared_ptr<ZRenderStateGroup>& RenderState() const { return renderState_; }

protected:

    ZVertexBuffer::ptr bufferData_;

    std::shared_ptr<ZRenderStateGroup> renderState_;

    static ZIDSequence idGenerator_;

};

class ZMesh2D : public ZMesh
{

public:

    ZMesh2D(const ZVertex2DDataOptions& vertexData)
        : ZMesh(), vertexData_(vertexData)
    { }
    ~ZMesh2D() {}

    void Initialize() override;

    ZVertex2DList Vertices() const { return vertexData_.vertices; }
    bool Instanced() const { return vertexData_.instanced.count > 1; }

    static std::shared_ptr<ZMesh2D> NewQuad();
    static std::shared_ptr<ZMesh2D> NewScreenTriangle();

protected:

    ZVertex2DDataOptions vertexData_;

};

class ZMesh3D : public ZMesh
{

    friend class ZModel;
    friend class ZModelImporter;

public:

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
