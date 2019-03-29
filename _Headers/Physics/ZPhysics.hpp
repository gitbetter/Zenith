/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZPhysics.hpp

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
#include "btBulletDynamicsCommon.h"
#include "ZCommon.hpp"
#include "ZPhysicsDebug.hpp"
#include "ZObjectForceRegistry.hpp"
#include "ZProcess.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZPhysics : public ZProcess {

private:

  std::unique_ptr<ZPhysicsDebug> debugDrawer_;
  std::unique_ptr<ZObjectForceRegistry> registry_;
  btDefaultCollisionConfiguration* collisionConfig_ = nullptr;
  btCollisionDispatcher* dispatcher_ = nullptr;
  btBroadphaseInterface* overlappingPairCache_ = nullptr;
  btSequentialImpulseConstraintSolver* solver_ = nullptr;
  btDiscreteDynamicsWorld* dynamicsWorld_ = nullptr;

public:

  ZPhysics() { }
  ~ZPhysics() { }

  void Initialize() override;

  void Update() override;

  ZObjectForceRegistry* Registry() { return registry_.get(); }

  void AddRigidBody(std::shared_ptr<btRigidBody> body);

  void HandleRaycastEvent(std::shared_ptr<ZEvent> event);

  ZGameObject* Raycast(glm::vec3 start, glm::vec3 direction);

  void DebugDraw();

  void CleanUp() override;

};
