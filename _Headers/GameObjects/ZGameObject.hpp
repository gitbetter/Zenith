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
  ZGameObject(glm::vec4 position = glm::vec4(glm::vec3(0.0f), 1.0f), glm::vec4 up = ZEngine::WORLD_UP, glm::vec4 rotation = glm::vec4(0.f, -90.f, 0.f, 0.f))
  : position_(position), eulerRotation_(rotation), front_(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)), up_(up) { CalculateTangentBasis(); }
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

  void SetPosition(glm::vec4 position) { position_ = position; }
  void SetRotation(glm::vec4 rotation) { eulerRotation_ = rotation; }
  void SetFrontVector(glm::vec4 front);

  const glm::vec4& GetPosition() { return position_; }
  const glm::vec4& GetRotation() { return eulerRotation_; }
  const glm::vec4& GetFrontVector() { return front_; }
  const glm::vec4& GetUpVector() { return up_; }
  const glm::vec4& GetRightVector() { return right_; }

protected:
  glm::vec4 position_, eulerRotation_, front_, up_, right_;
  ZGame* game_;

  void UpdateFrontVectorRotation();
};
