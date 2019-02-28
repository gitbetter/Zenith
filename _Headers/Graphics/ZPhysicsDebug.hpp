//
//  ZPhysicsDebug.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "btBulletDynamicsCommon.h"
#include "ZCommon.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZPhysicsDebug : public btIDebugDraw {

public:

  void Initialize();

protected:

  void drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
  void drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override { }
  void reportErrorWarning (const char *warningString) override { }
  void draw3dText (const btVector3 &location, const char *textString) override { }
  void setDebugMode (int debugMode) override { }
  int getDebugMode () const override { }

private:

  ZShader* shader_ = nullptr;

};