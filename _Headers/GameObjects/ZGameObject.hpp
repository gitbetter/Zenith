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
#include <glm/ext/quaternion_float.hpp>

// Forward Declarations
class ZGame;

// Class Definitions
class ZGameObject {
  friend class ZGame;

private:
  void CalculateTangentBasis();

public:
  ZGameObject(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = ZEngine::WORLD_UP)
  : position_(position), front_(glm::vec3(0.0f, 0.0f, -1.0f)), up_(up) { CalculateTangentBasis(); }
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

  void SetPosition(glm::vec3 position) { position_ = position; }
  void SetRotation(glm::quat rotation) { rotation_ = rotation; }
  void SetFrontVector(glm::vec3 front);

  const glm::vec3& GetPosition() { return position_; }
  const glm::quat& GetRotation() { return rotation_; }
  const glm::vec3& GetFrontVector() { return front_; }
  const glm::vec3& GetUpVector() { return up_; }
  const glm::vec3& GetRightVector() { return right_; }

protected:
  glm::vec3 position_, front_, up_, right_;
  glm::quat rotation_;
  ZGame* game_;
};
