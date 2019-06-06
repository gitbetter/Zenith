/*
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
   ZTrigger.cpp
 
   Created by Adrian Sanchez on 23/04/19.
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

#include "ZTrigger.hpp"
#include "ZPhysicsComponent.hpp"

ZTrigger::ZTrigger() : ZGameObject(glm::vec3(0.f)) {
    properties_.renderPass = ZRenderPass::Invisible;
	id_ = "ZTR_" + zenith::IDSequence()->Next();
}

void ZTrigger::Initialize() {
    ZGameObject::Initialize();
    
    std::shared_ptr<ZPhysicsComponent> physicsComp(new ZPhysicsComponent);
    physicsComp->Initialize("Trigger", "Box", 0.f, Position(), Scale(), Orientation());
    physicsComp->DisableCollisionResponse();
    
    AddComponent(physicsComp);
}

void ZTrigger::Initialize(std::shared_ptr<ZOFNode> root) {
    ZGameObject::Initialize(root);
    Initialize();
}
