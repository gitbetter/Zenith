/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZPhysicsFactory.hpp

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

#pragma once

// Includes
#include "btBulletDynamicsCommon.h"
#include "ZCommon.hpp"
#include "ZCollider.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZPhysicsFactory {

  using ZColliderCreator = std::shared_ptr<ZCollider> (ZPhysicsFactory::*)(std::vector<float> params);

public:

  ZPhysicsFactory();
  ~ZPhysicsFactory() { };

	std::shared_ptr<ZCollider> CreateCollider(std::string type, std::vector<float> size);

	std::shared_ptr<ZCollider> CreateBoxCollider(std::vector<float> params);
	std::shared_ptr<ZCollider> CreateSphereCollider(std::vector<float> params);
	std::shared_ptr<ZCollider> CreateCapsuleCollider(std::vector<float> params);

protected:

  std::map<std::string, ZColliderCreator> colliderCreators_;

};
