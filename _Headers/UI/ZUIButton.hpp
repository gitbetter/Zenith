//
//  ZUIButton.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZUIElement.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZUIButton : public ZUIElement {

public:

  ZUIButton(glm::vec2 position = glm::vec2(0.1f), glm::vec2 scale = glm::vec2(0.07f, 0.03f));
  ~ZUIButton() { }

  void Initialize(ZOFNode* root) override;

  void Draw(ZShader* shader) override;
  ZMeshUI ElementShape() override;

};
