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

// Includes
#include "ZProcess.hpp"
#include "ZBuffer.hpp"

// Forward Declarations
class ZShader;
class ZMaterial;

// Class and Data Structure Definitions
class ZMesh : public ZProcess
{

public:

    ZMesh(const ZMeshDrawStyle& drawStyle = ZMeshDrawStyle::Triangle)
        : drawStyle_(drawStyle)
    { }
    virtual ~ZMesh() {}

    virtual void Initialize() = 0;

    virtual void Render(const std::shared_ptr<ZShader>& shader, const std::shared_ptr<ZMaterial>& material) = 0;

    virtual ZMeshDrawStyle DrawStyle() const { return drawStyle_; };

protected:

    ZBuffer::ptr bufferData_;
    ZMeshDrawStyle drawStyle_;

    static ZIDSequence idGenerator_;

};

class ZMesh2D : ZMesh
{

public:

    ZMesh2D() {}
    ZMesh2D(const ZVertex2DDataOptions& vertexData, const ZMeshDrawStyle& drawStyle = ZMeshDrawStyle::Triangle)
        : ZMesh(drawStyle), vertexData_(vertexData)
    { }
    ~ZMesh2D() {}

    void Initialize() override;

    void Render(const std::shared_ptr<ZShader>& shader, const std::shared_ptr<ZMaterial>& material = nullptr) override;

    ZVertex2DList Vertices() const { return vertexData_.vertices; }

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

    ZMesh3D(const ZVertex3DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle);
    ~ZMesh3D();

    void Initialize() override;

    void Render(const std::shared_ptr<ZShader>& shader, const std::shared_ptr<ZMaterial>& material) override;

    const ZInstancedDataOptions& InstanceData() const { return vertexData_.instanced; }
    const ZVertex3DList& Vertices() const { return vertexData_.vertices; }
    const std::vector<unsigned int>& Indices() const { return vertexData_.indices; }

    void SetInstanceData(const ZInstancedDataOptions& data);
    void SetVertices(const ZVertex3DList& data);
    void SetIndices(const std::vector<unsigned int>& data);

private:

    ZVertex3DDataOptions vertexData_;

};
