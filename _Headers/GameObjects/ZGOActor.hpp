//
//  ZGOActor.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <iostream>
#include "ZGameObject.hpp"

// Forward Declarations
class ZGraphicsComponent;

// Class and Data Structure Definitions
class ZGOActor : public ZGameObject {
private:
  ZGraphicsComponent* graphicsComponent_ = nullptr;

public:
  ZGOActor(ZGraphicsComponent* graphicsComponent) : graphicsComponent_(graphicsComponent) { }
  ~ZGOActor() { }

  virtual void Update() override { };
  virtual void Render(float frameMix) override;
protected:

};
