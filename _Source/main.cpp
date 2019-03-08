//
//  main.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/24/19.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
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
  auto game = std::make_shared<ZGame>();

  // Initialize the engine before anything else
  ZEngine::Initialize(game, 1260, 800);

  // Ater initializing the engine, we can access the underlying UI subsystem to register fonts
  // TODO: Add a name field to this method to allow fonts to have arbitrary, unique names
  ZEngine::UI()->RegisterFont("Assets/Fonts/earth_orbiter/earthorbiter.ttf");

  // Enable our UI cursor
  ZEngine::UI()->EnableCursor();

  // Parse the ZOF file and create the resources
  ZGameObjectMap gameObjects = ZEngine::LoadZOF("basic_scene.zof");
  for (ZGameObjectMap::iterator it = gameObjects.begin(); it != gameObjects.end(); it++) {
    game->AddGameObject(it->second);
  }

  // Register the camera component so it receives input events
  std::shared_ptr<ZGameObject> activeCamera = game->ActiveCamera();
  if (activeCamera != nullptr)
    ZEngine::Input()->Register(activeCamera->FindComponent<ZCameraComponent>());

  // Now add some lights, because it's dark in here.
  game->AddGameObjects({std::shared_ptr<ZLight>(new ZLight(ZLightType::Directional))});

  // Let's add some UI components to the UI system to test
  auto uiButton = std::make_shared<ZUIButton>(glm::vec2(0.1f), glm::vec2(0.06f, 0.03f));
  uiButton->SetColor(glm::vec4(36.f/255.f, 37.f/255.f, 42.f/255.f, 1.f));
  uiButton->On(ZEventType::FirePress, [&]{
    uiButton->SetColor(glm::vec4(1.f));
  });

  // We can nest UI components by adding children to them, such as is done here
  // where some text is embedded in a button
  // TODO: Make sure the child always translates relative to the parent
  auto uiText = std::make_shared<ZUIText>("Zenith", "earthorbiter", 0.8f, glm::vec2(0.f), glm::vec2(0.06f, 0.03f));
  uiText->SetColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.f));
  uiButton->AddChild(uiText);
  uiText->Translate(glm::vec2(0.012f, 0.f));

  ZEngine::UI()->AddElements({uiButton});

  // Now it's time to add a skybox. Easy, but note, this should be the last visible game object we add.
  // The depth value of the skybox will always be 1.0, the max, so we must check it last to make sure it is
  // culled properly.
  // TODO: Maintain the skybox at the end of the game objects list internally
  game->SetDefaultSkybox();

  // Create the game and start the main game loop. Nothing beyond this point will execute
  // for the duration of the game.
  game->RunGameLoop();

  return 0;
}
