//
//  ZGameObject.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZEngine.hpp"
#include <glm/glm.hpp>

// Forward Declarations
class ZGame;

// Class Definitions
class ZGameObject {
  friend class ZGame;

private:
  void CalculateTangentBasis();

public:
  ZGameObject(glm::vec3 position = glm::vec3(0.f, 1.f, 0.f), glm::vec3 rotation = glm::vec3(0.f, -90.f, 0.f))
  : position_(glm::vec4(position, 1.f)),
    eulerRotation_(glm::vec4(rotation, 1.f)),
    front_(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)),
    up_(ZEngine::WORLD_UP)
  { CalculateTangentBasis(); }
  virtual ~ZGameObject() { }

  virtual void Update() { }
  virtual void Render(float frameMix) { }

  virtual void HandleStrafe(float controlThrow) { }
  virtual void HandleUpDown(float controlThrow) { }
  virtual void HandleForwardBack(float controlThrow) { }
  virtual void HandlePitch(float controlThrow) { }
  virtual void HandleYaw(float controlThrow) { }
  virtual void HandleFire() { }
  virtual void HandleEscape() { }

  void SetPosition(glm::vec3 position) { position_ = glm::vec4(position, 1.f); }
  void SetRotation(glm::vec3 rotation) { eulerRotation_ = glm::vec4(rotation, 0.f); }
  void SetFrontVector(glm::vec3 front);

  const glm::vec3 GetPosition() { return glm::vec3(position_); }
  const glm::vec3 GetRotation() { return glm::vec3(eulerRotation_); }
  const glm::vec3 GetFrontVector() { return glm::vec3(front_); }
  const glm::vec3 GetUpVector() { return glm::vec3(up_); }
  const glm::vec3 GetRightVector() { return glm::vec3(right_); }

protected:
  glm::vec4 position_, eulerRotation_, front_, up_, right_;
  ZGame* game_;

  void UpdateFrontVectorRotation();
};
