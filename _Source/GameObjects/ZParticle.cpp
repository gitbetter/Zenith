/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZParticle.cpp

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

#include "ZParticle.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZPhysicsComponent.hpp"

void ZParticle::Initialize() {
  // TODO: Uncomment once approriate model and shader is chosen for
  // displaying a particle
  // if (FindComponent<ZGraphicsComponent>() == nullptr) {
  //   AddComponent(new ZGraphicsComponent(the-model, the-shader));
  // }

  if (FindComponent<ZPhysicsComponent>() == nullptr) {
    AddComponent(std::shared_ptr<ZPhysicsComponent>(new ZPhysicsComponent));
  }
}

void ZParticle::Update() {
  ZGameObject::Update();
  age_ -= zenith::UPDATE_STEP_SIZE;
}

void ZParticle::Render(ZRenderOp renderOp) {
  // TODO: Render a particle as a quad that rotates with the camera
}
