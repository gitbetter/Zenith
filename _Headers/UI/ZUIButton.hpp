//
//  ZUIButton.hpp
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
class ZUIButton : public ZUIElement {
private:
public:
  ZUIButton(glm::vec2 position = glm::vec2(0.1f), glm::vec2 scale = glm::vec2(0.07f, 0.03f));
  virtual ~ZUIButton() { }

  void Render(ZShader* shader) override;
  ZMeshUI ElementShape() override;
protected:

};
