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

#include "ZEventAgent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZObjectDragEvent.hpp"

#include "ZOFParser.hpp"

std::shared_ptr<ZGame> game;
std::shared_ptr<ZUIButton> button;

void onButtonPress(std::shared_ptr<ZEvent> event);
void onButtonDrag(std::shared_ptr<ZEvent> event);

// TODO: How can we identify model meshes and add materials to them independently?
int main(int argc, const char * argv[]) {
  // Create a new game instance
  game = std::make_shared<ZGame>();

  // Initialize the engine before anything else
  ZEngine::Initialize(game, 1260, 800);

  // Ater initializing the engine, we can access the underlying UI subsystem to register fonts
  // TODO: Add a name field to this method to allow fonts to have arbitrary, unique names
  ZEngine::UI()->RegisterFont("Assets/Fonts/earth_orbiter/earthorbiter.ttf");

  // Parse the ZOF file and create the resources
  ZOFLoadResult zofResult = ZEngine::LoadZOF("basic_scene.zof");
  for (ZGameObjectMap::iterator it = zofResult.gameObjects.begin(); it != zofResult.gameObjects.end(); it++) {
    game->AddGameObject(it->second);
  }

  for (ZUIElementMap::iterator it = zofResult.uiElements.begin(); it != zofResult.uiElements.end(); it++) {
    if (std::dynamic_pointer_cast<ZUICursor>(it->second))
      ZEngine::UI()->SetCursor(std::dynamic_pointer_cast<ZUICursor>(it->second));
    else
      ZEngine::UI()->AddElement(it->second);
  }

  // Now add some lights, because it's dark in here.
  game->AddGameObjects({std::shared_ptr<ZLight>(new ZLight(ZLightType::Directional))});

  // We can register delegate methods for specific UI events
  button = ZEngine::UI()->FindElement<ZUIButton>("ZUI_01");
  ZEventDelegate pressDelegate(&onButtonPress);
  ZEventDelegate dragDelegate(&onButtonDrag);
  if (button) {
    ZEngine::EventAgent()->AddListener(pressDelegate, ZObjectSelectedEvent::Type);
    ZEngine::EventAgent()->AddListener(dragDelegate, ZObjectDragEvent::Type);
  }

  // Now it's time to add a skybox. Easy, but note, this should be the last visible game object we add.
  // The depth value of the skybox will always be 1.0, the max, so we must check it last to make sure it is
  // culled properly.
  // TODO: Maintain the skybox at the end of the game objects list internally
  game->SetDefaultSkybox();

  // Create the game and start the main game loop. Nothing beyond this point will execute
  // for the duration of the game.
  game->RunGameLoop();

  // We make sure to deregister delegates before objects are destroyed to avoid
  // dangling pointers in the FastDelegate implementation
  if (button) {
    ZEngine::EventAgent()->RemoveListener(pressDelegate, ZObjectSelectedEvent::Type);
    ZEngine::EventAgent()->RemoveListener(dragDelegate, ZObjectDragEvent::Type);
  }

  return 0;
}

void onButtonPress(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZObjectSelectedEvent> fireEvent = std::static_pointer_cast<ZObjectSelectedEvent>(event);
  if (button->ID() == fireEvent->ObjectID()) {
    if (button->Selected()) {
      button->Deselect();
      button->SetColor(glm::vec4(0.141f, 0.145f, 0.165f, 1.f));
      std::shared_ptr<ZUIText> text = button->Child<ZUIText>();
      text->SetColor(glm::vec4(1.f));
    } else {
      button->Select();
      button->SetColor(glm::vec4(1.f));
      std::shared_ptr<ZUIText> text = button->Child<ZUIText>();
      text->SetColor(glm::vec4(0.141f, 0.145f, 0.165f, 1.f));
    }
  }
}

void onButtonDrag(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZObjectDragEvent> dragEvent = std::static_pointer_cast<ZObjectDragEvent>(event);
  button->Translate(glm::vec2(dragEvent->DeltaX() * 100.f * ZEngine::DeltaTime(), 
                              -dragEvent->DeltaY() * 100.f * ZEngine::DeltaTime()));
}
