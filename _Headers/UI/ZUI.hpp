//
//  ZUI.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIElement.hpp"
#include <vector>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZUI {
private:
public:
  virtual ~ZUI() { }

  virtual void Draw() = 0;

protected:
  std::vector<ZUIElement*> elements_;
};
