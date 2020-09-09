/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZMesh3D.hpp

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

#pragma once

// Includes
#include "ZMesh.hpp"
#include "ZMaterial.hpp"

// Forward Declarations
class ZShader;

// Class Definitions
class ZMesh3D : public ZMesh
{

    friend class ZModel;
    friend class ZModelImporter;

private:

    ZVertex3DDataOptions vertexData_;

public:

    ZMesh3D(ZVertex3DDataOptions vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle);
    ~ZMesh3D();

    void Initialize() override;

    void Render(ZShader* shader, ZMaterial* material) override;

    void SetInstanceData(const ZInstancedDataOptions& data);
};
