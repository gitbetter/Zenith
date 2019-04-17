/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    main.cpp

    Created by Adrian Sanchez on 1/24/19.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

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

#include "ZAudio.hpp"
#include "ZAudioSource.hpp"

#include "ZEventAgent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZObjectDragEvent.hpp"

#include "ZResource.hpp"
#include "ZResourceCache.hpp"

#include "ZOFParser.hpp"

std::shared_ptr<ZGame> game;
std::shared_ptr<ZUIButton> button;

void onObjectSelect(std::shared_ptr<ZEvent> event);
void onObjectDrag(std::shared_ptr<ZEvent> event);

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
  ZOFLoadResult zofResult = ZEngine::LoadZOF("Assets/basic_scene.zof");
  for (ZGameObjectMap::iterator it = zofResult.gameObjects.begin(); it != zofResult.gameObjects.end(); it++) {
    game->AddGameObject(it->second);
  }

  for (ZUIElementMap::iterator it = zofResult.uiElements.begin(); it != zofResult.uiElements.end(); it++) {
    if (std::dynamic_pointer_cast<ZUICursor>(it->second))
      ZEngine::UI()->SetCursor(std::dynamic_pointer_cast<ZUICursor>(it->second));
    else
      ZEngine::UI()->AddElement(it->second);
  }

  // We can register delegate methods for specific UI events
  button = ZEngine::UI()->FindElement<ZUIButton>("ZUI_01");
  ZEventDelegate pressDelegate(&onObjectSelect);
  ZEventDelegate dragDelegate(&onObjectDrag);
  ZEngine::EventAgent()->AddListener(pressDelegate, ZObjectSelectedEvent::Type);
  ZEngine::EventAgent()->AddListener(dragDelegate, ZObjectDragEvent::Type);

  // Now it's time to add a skybox. Easy, but note, this should be the last visible game object we add.
  // The depth value of the skybox will always be 1.0, the max, so we must check it last to make sure it is
  // culled properly.
  // TODO: Maintain the skybox at the end of the game objects list internally
  game->SetDefaultSkybox();

	// Let's test the audio system by adding some background music to the scene.
	ZResource bgAmbient("Assets/Sounds/wind.ogg");
	auto ambientHandle = ZEngine::ResourceCache()->GetHandle(&bgAmbient);
	if (ambientHandle) {
		auto audioSource = ZEngine::Audio()->NewAudioSource(ambientHandle);
		audioSource->Play(35, true);
	}

  // Create the game and start the main game loop. Nothing beyond this point will execute
  // for the duration of the game.
  game->RunGameLoop();

  // We make sure to deregister delegates before objects are destroyed to avoid
  // dangling pointers in the FastDelegate implementation
  ZEngine::EventAgent()->RemoveListener(pressDelegate, ZObjectSelectedEvent::Type);
  ZEngine::EventAgent()->RemoveListener(dragDelegate, ZObjectDragEvent::Type);

  // Make sure to clean up all resources after we're done
  ZEngine::CleanUp();

  return 0;
}

void onObjectSelect(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZObjectSelectedEvent> fireEvent = std::static_pointer_cast<ZObjectSelectedEvent>(event);
  if (button != nullptr) {
    if (button->ID() == fireEvent->ObjectID()) {
      if (button->Selected()) {
        button->Deselect();
        button->SetColor(glm::vec4(0.141f, 0.145f, 0.165f, 1.f));
        std::shared_ptr<ZUIText> text = button->Child<ZUIText>("ZUI_02");
        text->SetColor(glm::vec4(1.f));
      } else {
        button->Select();
        button->SetColor(glm::vec4(1.f));
        std::shared_ptr<ZUIText> text = button->Child<ZUIText>("ZUI_02");
        text->SetColor(glm::vec4(0.141f, 0.145f, 0.165f, 1.f));
      }
    }
  } else {
    if (game->GameObjects().find(fireEvent->ObjectID()) != game->GameObjects().end()) {
			std::shared_ptr<ZGameObject> object = game->GameObjects()[fireEvent->ObjectID()];
      std::shared_ptr<ZPhysicsComponent> comp = object->FindComponent<ZPhysicsComponent>();
      glm::vec3 force = game->ActiveCamera()->Front() * 1000.f;
      glm::vec3 position = glm::inverse(object->ModelMatrix()) * glm::vec4(fireEvent->Position(), 1.0);
      comp->AddForceAtPoint(force, position);
    }
  }
}

void onObjectDrag(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZObjectDragEvent> dragEvent = std::static_pointer_cast<ZObjectDragEvent>(event);
  if (button != nullptr) {
    button->Translate(glm::vec2(dragEvent->DeltaX() * 100.f * ZEngine::DeltaTime(), 
                                -dragEvent->DeltaY() * 100.f * ZEngine::DeltaTime()));
  }
}
