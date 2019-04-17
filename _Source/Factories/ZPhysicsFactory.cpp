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

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateCollider(std::string type, std::vector<float> size) {
  return (this->*colliderCreators_[type])(size);
}

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateBoxCollider(std::vector<float> params) {
  glm::vec3 extents(1.f, 1.f, 1.f);
  switch (params.size()) {
    case 1: extents = glm::vec3(params[0], 1.f, 1.f); break;
    case 2: extents = glm::vec3(params[0], params[1], 1.f); break;
    case 3: extents = glm::vec3(params[0], params[1], params[2]); break;
    default: break;
  }

	return std::make_shared<ZCollider>(new btBoxShape(btVector3(extents[0], extents[1], extents[2])), ZColliderType::Box);
}

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateSphereCollider(std::vector<float> params) {
  float radius = 1.f;
  switch (params.size()) {
    case 1: radius = params[0]; break;
    default: break;
  }

	return std::make_shared<ZCollider>(new btSphereShape(radius), ZColliderType::Sphere);
}

std::shared_ptr<ZCollider> ZPhysicsFactory::CreateCapsuleCollider(std::vector<float> params) {
  float radius = 1.0, height = 1.0;
  switch (params.size()) {
    case 1: radius = params[0]; break;
    case 2: height = params[1]; break;
    default: break;
  }

	return std::make_shared<ZCollider>(new btCapsuleShape(radius, height), ZColliderType::Capsule);
}