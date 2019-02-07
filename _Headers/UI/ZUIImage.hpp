//
//  ZUIImage.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIElement.hpp"
#include <glm/glm.hpp>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZUIImage : public ZUIElement {
private:
public:
  ZUIImage(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) { }
  virtual ~ZUIImage();

  virtual void Render() override { }
protected:

};
