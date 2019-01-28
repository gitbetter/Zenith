//
//  ZGame.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>

// Forward Declarations
class ZGameObject;
struct GLFWwindow;

// Class and Data Structure Definitions
class ZGame {
private:
  std::vector<ZGameObject*> gameObjects_;

public:
  ZGame();
  ~ZGame() { };

  void RunGameLoop();

protected:
  void Update();
  void Render(float frameMix);
};
