/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZParticleSystem.hpp

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
#include "ZGameObject.hpp"

// Forward Declarations
class ZParticle;

// Class and Data Structure Definitions
struct ZParticleRule
{
    float minAge = 0.5f;
    float maxAge = 5.f;
    float minVelocity = 5.f;
    float maxVelocity = 100.f;
    float damping = 0.1f;
};

class ZParticleSystem : public ZGameObject
{
private:

    std::vector<ZParticle*> particles_;
    bool isAlive_ = false;

public:

    ZParticleSystem(const glm::vec3& position = glm::vec3(0.f, 1.f, 0.f), const glm::quat& orientation = glm::quat(glm::vec3(0.f)))
        : ZGameObject(position, orientation) { }
    ZParticleSystem(std::initializer_list<ZParticleRule> rules);
    ~ZParticleSystem() {}

    void Start() { isAlive_ = true; }
    void Stop() { isAlive_ = false; }

    bool Alive() { return isAlive_; }

    void Update(double deltaTime) override;

    DECLARE_OBJECT_CREATORS(ZParticleSystem)

protected:


};
