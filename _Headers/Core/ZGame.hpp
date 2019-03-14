//
//  ZGame.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZGameObject.hpp"
#include <initializer_list>

// Forward Declarations
struct ZLight;
class ZSkybox;

// Class and Data Structure Definitions
class ZGame : public ZObject {
private:

  std::shared_ptr<ZSkybox> skybox_ = nullptr;
  ZGameObjectMap gameObjects_;
  std::map<std::string, std::shared_ptr<ZLight>> gameLights_;
  std::string activeCameraObject_;

  void Setup();
  void CleanUp();

public:

  ZGame();
  ~ZGame() { };

  void RunGameLoop();

  std::shared_ptr<ZGameObject> ActiveCamera();
  const std::map<std::string, std::shared_ptr<ZLight>>& GameLights() const { return gameLights_; }
  std::shared_ptr<ZSkybox> Skybox() const { return skybox_; }
  const ZGameObjectMap& GameObjects() const { return gameObjects_; }

  void AddGameObject(std::shared_ptr<ZGameObject> gameObject);
  void AddGameObjects(std::initializer_list<std::shared_ptr<ZGameObject>> gameObjects);

  void SetDefaultSkybox();

  void HandleQuit(std::shared_ptr<ZEvent> event);

  void MacDisplayHack(); // TODO: Remove later

protected:

  void Update() override;
  void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) override;
};
