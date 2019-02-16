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
#include <vector>
#include <initializer_list>

// Forward Declarations
class ZActor;
struct ZLight;

// Class and Data Structure Definitions
class ZGame : public ZObject {
private:
  std::map<std::string, ZGameObject*> gameObjects_;
  std::map<std::string, ZLight*> gameLights_;
  std::string activeCameraObject_;

  void CleanUp();

public:
  ZGame();
  ~ZGame() { };

  void RunGameLoop();

  ZGameObject* GetActiveCamera() const;
  const std::map<std::string, ZLight*>& GetGameLights() const { return gameLights_; }

  void AddGameObject(ZGameObject* gameObject);
  void AddGameObjects(std::initializer_list<ZGameObject*> gameObjects);

  void SetDefaultSkybox();

  void HandleEscape() override;

  void MacDisplayHack(); // TODO: Remove later

protected:
  void Update();
  void Render(float frameMix);
};
