//
//  ZEngine.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

// We'll use more of these in the future to define our graphics implementation.
// For now, we're only assuming OpenGL
#define OGL_IMP

// Includes
#include <glm/vec4.hpp>
#include <string>
#include <vector>

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
  static const unsigned int SHADOW_MAP_SIZE;
  static const glm::vec4 WORLD_UP;
  static const std::vector<std::string> DEFAULT_SKYBOX_CUBEMAP;

  static ZGraphics* GetGraphics();
  static ZInput* GetInput();
  static float DeltaTime();
  static float MilliSecondTime();

  static void Provide(ZGraphics& graphics);
  static void Provide(ZInput& input);
  // More provide overloads for different engine subsystems (i.e. physics, audio, input, etc.)
  static void SetDeltaTime(float deltaTime);
protected:
};
