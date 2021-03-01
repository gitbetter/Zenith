/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZSceneRoot.hpp

   Created by Adrian Sanchez on 19/04/19.
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
class ZMesh2D;
class ZRenderStateGroup;

// Class Definitions
class ZSceneRoot : public ZGameObject
{

public:

    ZSceneRoot(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : ZGameObject(position, orientation, scale) { }
    ZSceneRoot(const std::string& name = "Root");

    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override { ZGameObject::Initialize(root); }

    std::shared_ptr<ZMesh2D> ScreenTri();

    void Prepare(double deltaTime) override;
    void PrepareChildren(double deltaTime) override;
    bool IsVisible() override { return true; }

    DECLARE_OBJECT_CREATORS(ZSceneRoot)

private:

    ZGameObjectList publicChildren_;
    std::shared_ptr<ZRenderStateGroup> renderState_;

};
