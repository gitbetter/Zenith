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

#include "ZActor.hpp"
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

int main(int argc, const char * argv[]) {


  ZOFParser parser;
  ZOFTree* objectTree = parser.Parse("scene_example.zof");
  _Z(objectTree->ToString(), ZINFO);

  // Create a new game instance
  ZGame game;

  // TODO: OPTIONAL: Make window size a cross platform abstraction
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

  // Create the physics subsystem and provide it to the engine
  ZPhysics physics;
  ZEngine::Provide(physics);

  // TODO: Create the audio subsystem and provide it to the engine

  // Ater providing a UI subsystem, we can now register fonts
  // TODO: Add a name field to this method to allow fonts to have arbitrary, unique names
  ui.RegisterFont("Assets/Fonts/earth_orbiter/earthorbiter.ttf");

  // Register the main game object so it receives input events
  input.Register(&game);

  // Let's add a camera to the game
  ZGameObject camera(glm::vec3(-5.f, 10.f, 25.f));
  ZCameraComponent cameraComponent(ZCameraType::Perspective);
  cameraComponent.SetMovementStyle(ZCameraMovementStyle::Follow);
  camera.AddComponent(&cameraComponent);

  game.AddGameObject(&camera);

  // Register the camera component so it receives input events
  input.Register(&cameraComponent);

  // Create our primary shader
  ZShader shader("Assets/Shaders/Vertex/basic.vert", "Assets/Shaders/Pixel/basic.frag");

  // Now let's add some renderable game objects to test
  // TODO: How can we identify model meshes and add materials to them independently?
  ZModel* ground = ZModel::NewPlanePrimitive(glm::vec3(100.f, 0.f, 100.f));
  ZModel* cube1 = ZModel::NewCubePrimitive(glm::vec3(3.f));
  ZModel* cube2 = ZModel::NewCubePrimitive(glm::vec3(2.f));
  ZModel* cube3 = ZModel::NewCubePrimitive(glm::vec3(4.f));

  ZActor groundActor;
  ZActor cubeActor1(glm::vec3(0.f, 3.f, 0.f));
  ZActor cubeActor2(glm::vec3(-10.f, 20.f, 5.f));
  ZActor cubeActor3(glm::vec3(-8.f, 4.f, -10.f));

  // ... and add graphics components to them, with the newly created models and shaders
  ZGraphicsComponent groundGraphicsComp; groundGraphicsComp.Initialize(ground, &shader);
  ZGraphicsComponent cubeGraphicsComp1; cubeGraphicsComp1.Initialize(cube1, &shader);
  ZGraphicsComponent cubeGraphicsComp2; cubeGraphicsComp2.Initialize(cube2, &shader);
  ZGraphicsComponent cubeGraphicsComp3; cubeGraphicsComp3.Initialize(cube3, &shader);

  cubeGraphicsComp1.SetOutline();

  groundActor.AddComponent(&groundGraphicsComp);
  cubeActor1.AddComponent(&cubeGraphicsComp1);
  cubeActor2.AddComponent(&cubeGraphicsComp2);
  cubeActor3.AddComponent(&cubeGraphicsComp3);

  game.AddGameObjects({&cubeActor1, &cubeActor2, &cubeActor3, &groundActor});

  // Let's add some physics to some of the cubes
  ZPhysicsComponent physicsComp1;
  ZPhysicsComponent physicsComp2;
  physicsComp1.SetMass(2.f);
  physicsComp2.SetMass(2.f);

  cubeActor2.AddComponent(&physicsComp1);
  cubeActor3.AddComponent(&physicsComp2);

  ZGravityForce gravity(glm::vec3(0.f, -25.f, 0.f));
  physics.Registry()->Add(&cubeActor2, &gravity);

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
  ZUIText uiText("Zenith", "earthorbiter", 0.45f, glm::vec2(0.f), glm::vec2(0.06f, 0.03f));
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
