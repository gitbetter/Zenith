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
#include "ZGLInput.hpp"

#include "ZCameraComponent.hpp"

#include "ZGraphics.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZLight.hpp"

#include "ZUI.hpp"
#include "ZUIButton.hpp"
#include "ZUIImage.hpp"
#include "ZUIText.hpp"
#include "ZUICursor.hpp"

#include "ZPhysics.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZGravityForce.hpp"
#include "ZSpringForce.hpp"
#include "ZAnchoredSpringForce.hpp"
#include "ZObjectForceRegistry.hpp"

#include "ZOFParser.hpp"

// TODO: How can we identify model meshes and add materials to them independently?
int main(int argc, const char * argv[]) {
  // Create a new game instance
  ZGame game;

  // TODO: Create the subsystems in a ZEngine::Initialize method
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
  // Create the physics subsystem and provide it to the engine
  ZPhysics physics;
  ZEngine::Provide(physics);
  // TODO: Create the audio subsystem and provide it to the engine

  // Ater providing a UI subsystem, we can now register fonts
  // TODO: Add a name field to this method to allow fonts to have arbitrary, unique names
  ui.RegisterFont("Assets/Fonts/earth_orbiter/earthorbiter.ttf");

  // Register the main game object so it receives input events
  input.Register(&game);

  // Parse the ZOF file and create the resources
  ZGameObjectMap gameObjects = ZEngine::LoadZOF("scene_example.zof");
  for (ZGameObjectMap::iterator it = gameObjects.begin(); it != gameObjects.end(); it++) {
    game.AddGameObject(it->second);
  }

  // Let's add a camera to the game
  // ZGameObject camera(glm::vec3(-5.f, 10.f, 25.f));
  // ZCameraComponent cameraComponent(ZCameraType::Perspective);
  // cameraComponent.SetMovementStyle(ZCameraMovementStyle::Follow);
  // camera.AddComponent(&cameraComponent);
  //
  // game.AddGameObject(&camera);

  // Register the camera component so it receives input events
  ZGameObject* activeCamera = game.GetActiveCamera();
  if (activeCamera != nullptr) input.Register(activeCamera->FindComponent<ZCameraComponent>());

  // Create our primary shader
  ZShader shader;
  shader.Initialize("Assets/Shaders/Vertex/basic.vert", "Assets/Shaders/Pixel/basic.frag");

  ZModel* ground = ZModel::NewPlanePrimitive(glm::vec3(100.f, 0.f, 100.f));
  ZGameObject groundActor;
  ZGraphicsComponent groundGraphicsComp; groundGraphicsComp.Initialize(ground, &shader);
  groundActor.AddComponent(&groundGraphicsComp);
  game.AddGameObjects({&groundActor});
  //
  // // Let's add some physics to some of the cubes
  // ZPhysicsComponent physicsComp1;
  // ZPhysicsComponent physicsComp2;
  // physicsComp1.SetMass(2.f);
  // physicsComp2.SetMass(2.f);
  //
  // cubeActor2.AddComponent(&physicsComp1);
  // cubeActor3.AddComponent(&physicsComp2);
  //
  // ZGravityForce gravity(glm::vec3(0.f, -25.f, 0.f));
  // physics.Registry()->Add(&cubeActor2, &gravity);

  // Now add some lights, because it's dark in here.
  game.AddGameObjects({new ZLight(ZLightType::Directional)});

  // Let's add some UI components to the UI system to test
  ZUIButton uiButton(glm::vec2(0.1f), glm::vec2(0.06f, 0.03f));
  uiButton.SetColor(glm::vec4(36.f/255.f, 37.f/255.f, 42.f/255.f, 1.f));
  uiButton.On(ZEventType::FirePress, [&]{
    uiButton.SetColor(glm::vec4(1.f));
  });

  // We can nest UI components by adding children to them, such as is done here
  // where some text is embedded in a button
  // TODO: Make sure the child always translates relative to the parent
  ZUIText uiText("Zenith", "earthorbiter", 0.8f, glm::vec2(0.f), glm::vec2(0.06f, 0.03f));
  uiText.SetColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
  uiButton.AddChild(&uiText);
  uiText.Translate(glm::vec2(0.012f, 0.f));

  ui.AddElements({&uiButton});

  // Now it's time to add a skybox. Easy, but note, this should be the last visible game object we add.
  // The depth value of the skybox will always be 1.0, the max, so we must check it last to make sure it is
  // culled properly.
  // TODO: Maintain the skybox at the end of the game objects list internally
  game.SetDefaultSkybox();

  // Enable our UI cursor
  ui.EnableCursor();

  // Create the game and start the main game loop. Nothing beyond this point will execute
  // for the duration of the game.
  game.RunGameLoop();

  return 0;
}
