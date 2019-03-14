//
//  ZPhysicsDebug.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

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
