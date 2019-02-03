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
class ZCamera;
class ZActor;

// Class and Data Structure Definitions
class ZGame : public ZGameObject {
private:
  std::vector<ZGameObject*> gameObjects_;
  std::vector<ZCamera*> gameCameras_;
  int activeCameraIndex_;

public:
  ZGame();
  ~ZGame() { };

  void RunGameLoop();

  ZCamera* GetActiveCamera() const;

  void AddGameObject(ZGameObject* gameObject);
  void AddGameObjects(std::initializer_list<ZGameObject*> gameObjects);

  void HandleEscape() override;

protected:
  void Update() override;
  void Render(float frameMix) override;
};
