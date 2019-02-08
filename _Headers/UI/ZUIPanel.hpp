//
//  ZUIPanel.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
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
  ZUIPanel(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) { }
  virtual ~ZUIPanel();

  void Render(ZShader* shader) override { }
  ZMeshUI ElementShape() override { }
protected:

};
