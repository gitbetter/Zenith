//
//  ZGOCharacter.hpp
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
class ZGOCharacter : public ZGameObject {
private:
  ZGraphicsComponent* graphicsComponent_ = nullptr;

public:
  ZGOCharacter(ZGraphicsComponent* graphicsComponent);
  ~ZGOCharacter() { };

  virtual void Update() override { };
  virtual void Render(float frameMix) override;
protected:

};
