//
//  ZUIPanel.hpp
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
class ZUIPanel : public ZUIElement {
private:

public:

  ZUIPanel(glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(1.f)) : ZUIElement(position, scale) { }
  ~ZUIPanel() { }

  void Initialize(ZOFNode* root) override;

  void Draw(ZShader* shader) override { }
  ZMeshUI ElementShape() override { }

protected:

};
