//
//  main.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/24/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZDomain.hpp"
#include "ZGraphics.hpp"
#include "ZGLInput.hpp"
#include "ZUI.hpp"
#include "ZUIButton.hpp"
#include "ZUIImage.hpp"
#include "ZCamera.hpp"
#include "ZActor.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZLight.hpp"
#include "ZLogger.hpp"
#include <glm/glm.hpp>

int main(int argc, const char * argv[]) {
  // Create a new game instance
  ZGame game;

  // TODO: Make window size a cross platform abstraction
  // and require only an aspect ratio as the domain constructor param

  // Create a new domain and provide it to the engine
  ZDomain domain(1260, 800);
  ZEngine::Provide(domain);

  // Create the graphics subsystem and provide it to the engine
  ZGraphics graphics;
  ZEngine::Provide(graphics);

  // Create the input subsystem and provide it to the engine
  ZGLInput input;
  ZEngine::Provide(input);

  // Create the UI subsystem and provide it to the engine
  ZUI ui;
  ZEngine::Provide(ui);

  // TODO: Create the physics subsystem and provide it to the engine
  // TODO: Create the audio subsystem and provide it to the engine

  // Register the main game object so it receives input events
  input.Register(&game);

  // Let's add a camera to the game
  ZCamera camera(ZCameraType::Perspective, glm::vec3(0.0f, 2.0f, 4.0f));
  game.AddGameObject(&camera);

  // Register the camera so it receives input events
  input.Register(&camera);

  // Create our primary shader
  ZShader shader("Assets/Shaders/Vertex/basic.vert", "Assets/Shaders/Pixel/basic.frag");

  // Now let's add some renderable game objects to test
  // TODO: How can identify model meshes to add materials independently?
  ZModel* ground = ZModel::NewPlanePrimitive(glm::vec3(100.f, 0.f, 100.f));
  ZModel* cube1 = ZModel::NewCubePrimitive(glm::vec3(3.f));
  ZModel* cube2 = ZModel::NewCubePrimitive(glm::vec3(2.f));
  ZModel* cube3 = ZModel::NewCubePrimitive(glm::vec3(4.f));

  ZActor groundActor;
  ZActor cubeActor1(glm::vec3(0.f, 3.f, 0.f));
  ZActor cubeActor2(glm::vec3(-10.f, 2.f, 5.f));
  ZActor cubeActor3(glm::vec3(-8.f, 4.f, -10.f));

  // ... and add graphics components to them, with the newly created models and shaders
  ZGraphicsComponent groundGraphicsComp(ground, &shader);
  ZGraphicsComponent cubeGraphicsComp1(cube1, &shader);
  ZGraphicsComponent cubeGraphicsComp2(cube2, &shader);
  ZGraphicsComponent cubeGraphicsComp3(cube3, &shader);
  groundActor.AddComponent(&groundGraphicsComp);
  cubeActor1.AddComponent(&cubeGraphicsComp1);
  cubeActor2.AddComponent(&cubeGraphicsComp2);
  cubeActor3.AddComponent(&cubeGraphicsComp3);

  game.AddGameObjects({&cubeActor1, &cubeActor2, &cubeActor3, &groundActor});

  // Now it's time to add a skybox. Easy, but note, this should be the last visible game object we add.
  // The depth value of the skybox will always be 1.0, the max, so we must check it last to make sure it is
  // culled properly.
  // TODO: Maintain the skybox at the end of the game objects list internally
  game.SetDefaultSkybox();

  // Now add some lights, because it's dark in here.
  game.AddGameObject(new ZLight(ZLightType::Directional));

  // Let's add some UI components to the UI system to test
  ui.EnableCursor();

  // Create the game and start the main game loop. Nothing beyond this point will execute
  // for the duration of the game.
  game.RunGameLoop();

  return 0;
}
