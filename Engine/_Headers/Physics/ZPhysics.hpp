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
#include "ZCommon.hpp"
#include "ZProcess.hpp"

// Forward Declarations
class ZRigidBody;

// Class and Data Structure Definitions
class ZPhysics : public ZProcess {
    
public:
    
    virtual ~ZPhysics() { }
    
    virtual void Initialize() override;
    
    virtual void Update() override;
    
    virtual void CleanUp() override;
    
    virtual void AddRigidBody(std::shared_ptr<ZRigidBody> body) = 0;
    
    virtual ZRaycastHitResult Raycast(glm::vec3 start, glm::vec3 direction) = 0;
    
    virtual void DebugDraw() = 0;
    
protected:
    
    ZCollisionPairs previousTickCollisionPairs_;
    
};
