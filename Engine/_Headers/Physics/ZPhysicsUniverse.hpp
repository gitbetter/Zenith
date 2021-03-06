/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZPhysicsUniverse.hpp

   Created by Adrian Sanchez on 14/02/2019.
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

// Forward Declarations
class ZRigidBody;

// Class and Data Structure Definitions
class ZPhysicsUniverse : public ZProcess
{

public:

    virtual ~ZPhysicsUniverse() {}

    virtual void Initialize() override;

    virtual void Update(double deltaTime) override;

    virtual void CleanUp() override;

    virtual void AddRigidBody(std::shared_ptr<ZRigidBody> body) = 0;

    virtual void RemoveRigidBody(std::shared_ptr<ZRigidBody> body) = 0;

    virtual ZRaycastHitResult Raycast(const glm::vec3& start, const glm::vec3& direction, float t = 100.f) = 0;

    virtual void DebugDraw(const std::shared_ptr<ZScene>& scene) = 0;

protected:

    ZCollisionPairs previousTickCollisionPairs_;

};
