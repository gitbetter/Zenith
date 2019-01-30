//
//  ZEngine.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <glm/vec3.hpp>

// Forward Declarations
class ZGraphics;
class ZInput;

// Class Definitions
class ZEngine {
private:
  static ZGraphics* graphics_;
  static ZInput* input_;
  static float deltaTime_;
public:
  static const float MS_PER_UPDATE;
  static const int MAX_FIXED_UPDATE_ITERATIONS;
  static const glm::vec3 WORLD_UP;

  static ZGraphics* GetGraphics();
  static ZInput* GetInput();
  static float DeltaTime();

  static void Provide(ZGraphics& graphics);
  static void Provide(ZInput& input);
  // More provide overloads for different engine subsystems (i.e. physics, audio, input, etc.)
  static void SetDeltaTime(float deltaTime);
protected:
};
