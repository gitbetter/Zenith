//
//  ZUICursor.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZUIElement.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZUICursor : public ZUIElement {

private:

  float cursorSpeed_ = 20.f;

  void HandleMouseMove(std::shared_ptr<ZEvent> event);
  void HandleMousePress(std::shared_ptr<ZEvent> event);

public:

  ZUICursor(glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(25.f)) : ZUIElement(position, scale) { }
  ~ZUICursor() { }

  void Initialize(ZOFNode* root) override;

  void Draw(ZShader* shader) override;

  void SetCursorImage(std::string path);
  void SetColor(glm::vec4 color) override;
  void SetCursorSpeed(float sensitivity) { cursorSpeed_ = sensitivity; }

};