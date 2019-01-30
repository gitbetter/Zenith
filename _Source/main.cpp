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
#include "ZGLInput.hpp"
#include "ZCamera.hpp"
#include "ZActor.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include <glm/glm.hpp>

int main(int argc, const char * argv[]) {
  // Create a new game instance
  ZGame game;
  // Create the graphics subsystem and provide it to the engine
  ZGLGraphics graphics(800, 600);
  ZEngine::Provide(graphics);
  // Create the input subsystem and provide it to the engine
  ZGLInput input;
  ZEngine::Provide(input);
  // TODO: Create the physics subsystem and provide it to the engine
  // TODO: Create the audio subsystem and provide it to the engine

  // Register the top level game ZGameObject to listen for input events
  input.Register(game);

  // Let's add a camera to the game
  ZCamera camera;
  game.AddGameObject(&camera);

  // Now let's add a renderable game object to test
  ZModel susanne("Resources/Models/Susanne.dae");
  ZShader shader("Resources/Shaders/Vertex/basic.vertex", "Resources/Shaders/Pixel/basic.pixel");
  ZActor actor(glm::vec3(0.3f, 0.3f, -2.0f));

  ZGraphicsComponent graphicsComp(&susanne, &shader);
  actor.SetGraphicsComponent(&graphicsComp);

  game.AddGameObject(&actor);

  // Create the game and start the main game loop
  game.RunGameLoop();

  return 0;
}
