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

  // Register the main game object so it receives input events
  input.Register(game);

  // Let's add a camera to the game
  ZCamera camera(glm::vec4(0.0f, 0.0f, 4.0f, 1.0f));
  game.AddGameObject(&camera);

  // Register the camera so it receives input events
  input.Register(camera);

  // Now let's add a renderable game object to test
  ZModel susanne("Resources/Models/shanty_room.dae");
  ZShader shader("Resources/Shaders/Vertex/basic.vert", "Resources/Shaders/Pixel/basic.frag");
  ZActor actor;

  ZGraphicsComponent graphicsComp(&susanne, &shader);
  actor.SetGraphicsComponent(&graphicsComp);

  game.AddGameObject(&actor);

  // Create the game and start the main game loop. Nothing beyond this point will execute.
  game.RunGameLoop();

  return 0;
}
