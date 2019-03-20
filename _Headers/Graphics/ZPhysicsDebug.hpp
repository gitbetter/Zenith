/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZPhysicsDebug.hpp

    Created by Adrian Sanchez on 26/02/2019.
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
#include "ZShader.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZPhysicsDebug : public btIDebugDraw {

public:

  void Initialize();

protected:

  void drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
  void drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override { }
  void reportErrorWarning (const char *warningString) override { }
  void draw3dText (const btVector3 &location, const char *textString) override { }
  void setDebugMode (int debugMode) override { debugMode_ = debugMode; }
  int getDebugMode () const override { return debugMode_; }

private:

  std::unique_ptr<ZShader> shader_;
  int debugMode_;

};
