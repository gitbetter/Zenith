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
#include "ZCommon.hpp"

// Forward Declarations
class ZGraphics;
class ZInput;
class ZDomain;
class ZUI;
class ZPhysics;
class ZIDSequence;
class ZGOFactory;
class ZGraphicsFactory;
class ZGame;

// Class Definitions
class ZEngine {
private:

  static ZGame* currentGame_;
  static ZDomain* domain_;
  static ZGraphics* graphics_;
  static ZInput* input_;
  static ZUI* ui_;
  static ZPhysics* physics_;

  static ZGOFactory* gameObjectFactory_;
  static ZGraphicsFactory* graphicsFactory_;

  static float deltaTime_;
  static ZIDSequence* idGenerator_;

public:

  static const float UPDATE_STEP_SIZE;
  static const int MAX_FIXED_UPDATE_ITERATIONS;
  static const unsigned int SHADOW_MAP_SIZE;
  static const unsigned int CUBE_MAP_SIZE;
  static const glm::vec4 WORLD_UP;
  static const std::vector<std::string> DEFAULT_SKYBOX_CUBEMAP;
  static const std::string DEFAULT_HDR_CUBEMAP;

  static void Initialize(ZGame* game, int windowWidth, int windowHeight);

  static ZGame* Game();
  static ZDomain* Domain();
  static ZGraphics* Graphics();
  static ZInput* Input();
  static ZUI* UI();
  static ZPhysics* Physics();
  static ZIDSequence* IDSequence();

  static ZGOFactory* GameObjectFactory();
  static ZGraphicsFactory* GraphicsFactory();

  static float DeltaTime();
  static float MilliSecondTime();

  static void Provide(ZDomain* domain);
  static void Provide(ZGraphics* graphics);
  static void Provide(ZInput* input);
  static void Provide(ZUI* ui);
  static void Provide(ZPhysics* physics);
  // More provide overloads for different engine subsystems (i.e. audio, animation, etc.)
  static void SetDeltaTime(float deltaTime);

  static ZGameObjectMap LoadZOF(std::string zofPath);

  static void CleanUp();
protected:

};
