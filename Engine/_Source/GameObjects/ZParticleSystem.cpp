/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZParticleSystem.cpp

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

#include "ZParticleSystem.hpp"
#include "ZAssets.hpp"

ZParticleSystem::ZParticleSystem(std::initializer_list<ZParticleRule> rules)
{
	for (const ZParticleRule& rule : rules)
	{
		particles.push_back(ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Particle));
	}
}

void ZParticleSystem::OnUpdate(double deltaTime)
{
// TODO: Create a particle based on a particle rule and set its age randomly between the min and max ages.
// Also set the velocity randomly between the min and max velocities. If emitting randomly,
// pick randomly from the list of provided particles rules to create the particle.
// Make sure emissionRate time has elapsed before the previous emission to emit a new particle.
}