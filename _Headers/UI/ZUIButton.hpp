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
// class SomeClass;

// Class and Data Structure Definitions
class ZUIButton : public ZUIElement {
private:
public:
  ZUIButton(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) { }
  virtual ~ZUIButton();

  virtual void Render() override { }
protected:

};
