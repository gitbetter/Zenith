//
//  main.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/24/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZGLWindow.hpp"
#include "ZGLGraphics.hpp"

int main(int argc, const char * argv[]) {
  // Create the graphics subsystem and provide it to the engine
  ZGLGraphics graphics(800, 600);
  ZEngine::Provide(graphics);
  // TODO: Create the input subsystem and provide it to the engine
  // TODO: Create the physics subsystem and provide it to the engine
  // TODO: Create the audio subsystem and provide it to the engine

  // Create the game and start the main game loop
  ZGame game;
  game.RunGameLoop();

  return 0;
}
