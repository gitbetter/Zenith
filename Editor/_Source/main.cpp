/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    main.cpp
 
    Created by Adrian Sanchez on 17/05/19.
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

#include "ZEditor.hpp"
#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZScene.hpp"
#include "ZUI.hpp"
#include "ZProcessRunner.hpp"


// TODO: How can we identify model meshes and add materials to them independently?
int main(int argc, const char * argv[]) {
    // Create a new game instance
    std::shared_ptr<ZGame> game = std::make_shared<ZGame>();
	std::shared_ptr<ZEditor> editor = std::make_shared<ZEditor>();
    
    // Initialize the engine before anything else
    zenith::Initialize(game, 1920, 1080);
    
    // Ater initializing the engine, we can access the underlying UI subsystem to register fonts
    // TODO: Add a name field to this method to allow fonts to have arbitrary, unique names
    zenith::UI()->RegisterFont("Assets/Fonts/earth_orbiter/earthorbiter.ttf");
    
    // Load our scene using description files
    zenith::LoadScene({ "Assets/basic_scene.zof" });

	// Add our editor instance to the process runner so that it can
	// receive update ticks
	zenith::ProcessRunner()->AttachProcess(editor, ZPriority::Low);
    
    // Create the game and start the main game loop. Nothing beyond this point will execute
    // for the duration of the game.
    game->RunGameLoop();
    
    // Make sure to clean up all resources after we're done
    zenith::CleanUp();
    
    return 0;
}
