//
//  ZGame.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGameObject.hpp"
#include <vector>

// Forward Declarations

// Class and Data Structure Definitions
class ZGame : public ZGameObject {
private:
  std::vector<ZGameObject*> gameObjects_;

public:
  ZGame();
  ~ZGame() { };

  void RunGameLoop();

  void HandleYaw(float controlThrow) override;

protected:
  void Update() override;
  void Render(float frameMix) override;
};
