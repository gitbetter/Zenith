/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZMesh2D.hpp

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
#include "ZMesh.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZMesh2D : ZMesh
{
private:

public:

    ZMesh2D() {}
    ZMesh2D(const ZVertex2DDataOptions& vertexData) : vertexData_(vertexData) {}
    ~ZMesh2D() {}

    void Initialize() override;

    void Render(ZShader* shader, ZMaterial* material = nullptr) override;

    std::vector<ZVertex2D> Vertices() const { return vertexData_.vertices; }

    static std::shared_ptr<ZMesh2D> NewQuad();

protected:

    ZVertex2DDataOptions vertexData_;
};
