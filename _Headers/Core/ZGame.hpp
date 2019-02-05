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
struct ZLight;

// Class and Data Structure Definitions
class ZGame : public ZGameObject {
private:
  std::vector<ZGameObject*> gameObjects_;
  std::vector<ZCamera*> gameCameras_;
  std::vector<ZLight*> gameLights_;
  int activeCameraIndex_;

public:
  ZGame();
  ~ZGame() { };

  void RunGameLoop();

  ZCamera* GetActiveCamera() const;

  void AddGameObject(ZGameObject* gameObject);
  void AddGameObjects(std::initializer_list<ZGameObject*> gameObjects);

  void SetDefaultSkybox();

  void HandleEscape() override;

  void MacDisplayHack(); // TODO: Remove later

  // TODO: Use a std::map to store typeid(T).name as the key and T* as the value for faster lookups
  template<class T> std::vector<T*> FindGameObjects() {
    std::vector<T*> objects;
    for (ZGameObject* object : gameObjects_) {
      if (dynamic_cast<T*>(object))
        objects.push_back(dynamic_cast<T*>(object));
    }
    return objects;
  }

protected:
  void Update() override;
  void Render(float frameMix) override;
};
