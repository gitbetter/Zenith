/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGame.hpp

    Created by Adrian Sanchez on 27/01/2019.
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

#pragma once

// Includes
#include "ZGameObject.hpp"
#include <initializer_list>

// Forward Declarations
struct ZLight;
class ZSkybox;

// Class and Data Structure Definitions
class ZGame {
private:

  std::shared_ptr<ZSkybox> skybox_ = nullptr;
  ZGameObjectMap gameObjects_;
  ZLightMap gameLights_;
  std::string activeCameraObject_;

  void Setup();
  void CleanUp();

public:

  ZGame();
  ~ZGame() { };

  void RunGameLoop();

  std::shared_ptr<ZGameObject> ActiveCamera();
  const ZLightMap& GameLights() const { return gameLights_; }
  std::shared_ptr<ZSkybox> Skybox() const { return skybox_; }
  const ZGameObjectMap& GameObjects() const { return gameObjects_; }

  void AddGameObject(std::shared_ptr<ZGameObject> gameObject);
  void AddGameObjects(std::initializer_list<std::shared_ptr<ZGameObject>> gameObjects);

  void SetActiveCamera(std::shared_ptr<ZGameObject> gameObject);
  void SetDefaultSkybox();

  void HandleQuit(std::shared_ptr<ZEvent> event);

  // TODO: Remove later when we figure out why some screens don't refresh on startup
  void MacDisplayHack(); 

protected:

  void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR);
  
};
