//
//  ZParticle.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 13/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGameObject.hpp"
#include "ZGraphics.hpp"

// Forward Declarations
class ZGraphicsComponent;
class ZPhysicsComponent;

// Class and Data Structure Definitions
class ZParticle : public ZGameObject {
private:

  float age_;
  bool active_;
  
public:

  ZParticle() : ZGameObject() { }
  ~ZParticle() { }

  void Initialize();

  void Update() override;
  void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) override;

  float Age() { return age_; }
  bool Active() { return active_; }

protected:

  ZGraphicsComponent* graphicsComponent_ = nullptr;
  ZPhysicsComponent* physicsComponent_ = nullptr;

};
