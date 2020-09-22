/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZGrass.hpp

   Created by Adrian Sanchez on 09/08/2020.
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
#include "ZGameObject.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZGrass : public ZGameObject
{

private:

    static constexpr unsigned int const& cPolygonCount = 3;

    std::shared_ptr<ZGraphicsComponent> graphicsComp_;
    std::string textureId_;
    unsigned int instanceCount_;
    std::vector<std::shared_ptr<ZModel>> polygons_;
    glm::vec3 windDirection_;
    float windStrength_;
    float time_;

public:

    ZGrass(unsigned int instances = 0);
    ~ZGrass() {}

    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override;
    void Render(ZRenderOp renderOp = ZRenderOp::Color) override;
    bool IsVisible() override { return true; }

    void TrimPatch(const glm::vec3& position, const glm::vec3& size);

private:

    void UpdateVertexNormals(std::shared_ptr<ZModel>& model);

};
