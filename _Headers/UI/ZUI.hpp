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
class ZGraphicsStrategy;

// Class and Data Structure Definitions
class ZUI {
private:
public:
  virtual ~ZUI() { }

  void Initialize();

  void Draw();

protected:
  std::vector<ZUIElement*> elements_;
  ZGraphicsStrategy* graphicsStrategy_;
  ZShader* uiShader_;
};
