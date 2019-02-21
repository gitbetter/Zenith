//
//  ZGame.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGameObject.hpp"
#include <initializer_list>

// Forward Declarations
struct ZLight;

// Class and Data Structure Definitions
class ZGame : public ZObject {
private:
  ZGameObjectMap gameObjects_;
  std::map<std::string, ZLight*> gameLights_;
  std::string activeCameraObject_;

  void CleanUp();

public:
  ZGame();
  ~ZGame() { };

  void RunGameLoop();

  ZGameObject* GetActiveCamera();
  const std::map<std::string, ZLight*>& GetGameLights() const { return gameLights_; }

  void AddGameObject(ZGameObject* gameObject);
  void AddGameObjects(std::initializer_list<ZGameObject*> gameObjects);

  void SetDefaultSkybox();

  void HandleEscape() override;

  void MacDisplayHack(); // TODO: Remove later

protected:
  void Update() override;
  void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) override;
};
