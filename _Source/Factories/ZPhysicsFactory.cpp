/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZPhysicsFactory.cpp

    Created by Adrian Sanchez on 05/04/2019.
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

#include "ZPhysicsFactory.hpp"

ZPhysicsFactory::ZPhysicsFactory() {
    colliderCreators_["Box"] = &ZPhysicsFactory::CreateBoxCollider;
    colliderCreators_["Sphere"] = &ZPhysicsFactory::CreateSphereCollider;
    colliderCreators_["Capsule"] = &ZPhysicsFactory::CreateCapsuleCollider;
}

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateCollider(std::string type, glm::vec3 size) {
    return (this->*colliderCreators_[type])(size);
}

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateBoxCollider(glm::vec3 extents) {
	return std::make_shared<ZCollider>(new btBoxShape(btVector3(extents[0], extents[1], extents[2])), ZColliderType::Box);
}

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateSphereCollider(glm::vec3 extents) {
	return std::make_shared<ZCollider>(new btSphereShape(extents[0]), ZColliderType::Sphere);
}

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateCapsuleCollider(glm::vec3 extents) {
	return std::make_shared<ZCollider>(new btCapsuleShape(extents[0], extents[1]), ZColliderType::Capsule);
}
