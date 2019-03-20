/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZStiffSpringForce.cpp

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

#include "ZStiffSpringForce.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZEngine.hpp"

ZStiffSpringForce::ZStiffSpringForce(glm::vec3 anchor, float springConstant, float damping)
: anchor_(anchor), springConstant_(springConstant), damping_(damping) { }

void ZStiffSpringForce::UpdateForce(ZGameObject* object) {
  std::shared_ptr<ZPhysicsComponent> physicsComp = object->FindComponent<ZPhysicsComponent>();
  if (physicsComp == nullptr) return;

  if (!physicsComp->HasFiniteMass()) return;

  glm::vec3 position = object->Position();
  position -= anchor_;

  float gamma = 0.5f * glm::sqrt(4.f * springConstant_ - damping_ * damping_);
  if (gamma == 0.0f) return;

  glm::vec3 c = position * (damping_ / (2.f * gamma)) +
                physicsComp->Velocity() * (1.f / gamma);
  glm::vec3 target = position * glm::cos(gamma * ZEngine::UPDATE_STEP_SIZE) +
                     c * glm::sin(gamma * ZEngine::UPDATE_STEP_SIZE);
  target *= glm::exp(-0.5f * ZEngine::UPDATE_STEP_SIZE * damping_);

  glm::vec3 acceleration = (target - position) * (1.f / ZEngine::UPDATE_STEP_SIZE * ZEngine::UPDATE_STEP_SIZE) -
                           physicsComp->Velocity() * ZEngine::UPDATE_STEP_SIZE;

  glm::vec3 force = acceleration * physicsComp->Mass();
  physicsComp->AddForce(force);
}
