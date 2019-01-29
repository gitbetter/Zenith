//
//  ZGameObject.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <glm/vec4.hpp>

// Forward Declarations
class ZCommandComponent;

// Class Definitions
class ZGameObject {
private:
  glm::vec4 worldPosition_; // TODO: Set initially to zero vector

public:
  virtual ~ZGameObject() { }

  virtual void Update() { }
  virtual void Render(float frameMix) { }

  virtual void HandleStrafe(float controlThrow) { }
  virtual void HandleUpDown(float controlThrow) { }
  virtual void HandleForwardBack(float controlThrow) { }
  virtual void HandleFire() { }
  virtual void HandleEscape() { }

  void SetWorldPosition(glm::vec4 position) { worldPosition_ = position; }
  const glm::vec4& GetWorldPosition() { return worldPosition_; }

protected:
};
