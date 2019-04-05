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

btCollisionShape* ZPhysicsFactory::CreateCollider(std::string type, std::vector<btScalar> size) {
  return (this->*colliderCreators_[type])(size);
}

btCollisionShape* ZPhysicsFactory::CreateBoxCollider(std::vector<btScalar> params) {
  btVector3 extents(1.0, 1.0, 1.0);
  switch (params.size()) {
    case 1: extents = btVector3(params[0], 1.0, 1.0); break;
    case 2: extents = btVector3(params[0], params[1], 1.0); break;
    case 3: extents = btVector3(params[0], params[1], params[2]); break;
    default: break;
  }

  return new btBoxShape(extents);
}

btCollisionShape* ZPhysicsFactory::CreateSphereCollider(std::vector<btScalar> params) {
  btScalar radius = 1.0;
  switch (params.size()) {
    case 1: radius = params[0]; break;
    default: break;
  }

  return new btSphereShape(radius);
}

btCollisionShape* ZPhysicsFactory::CreateCapsuleCollider(std::vector<btScalar> params) {
  btScalar radius = 1.0, height = 1.0;
  switch (params.size()) {
    case 1: radius = params[0]; break;
    case 2: height = params[1]; break;
    default: break;
  }

  return new btCapsuleShape(radius, height);
}