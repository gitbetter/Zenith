/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZParticle.hpp

    Created by Adrian Sanchez on 13/02/2019.
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
class ZGraphicsComponent;
class ZPhysicsComponent;

// Class and Data Structure Definitions
class ZParticle : public ZGameObject
{
private:

    float age_;
    bool active_;

public:

    ZParticle(const glm::vec3& position = glm::vec3(0.f, 1.f, 0.f), const glm::quat& orientation = glm::quat(glm::vec3(0.f)), const glm::vec3& scale = glm::vec3(1.f))
        : ZGameObject(position, orientation, scale) { }
    ~ZParticle() {}

    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override { ZGameObject::Initialize(root); }

    void Update(double deltaTime) override;
    void Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp = ZRenderOp::Color) override;

    float Age() { return age_; }
    bool Active() { return active_; }

    DECLARE_OBJECT_CREATORS(ZParticle)

};
