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
class ZUICursor;

// Class and Data Structure Definitions
class ZUI {
private:
public:
  ~ZUI() { }

  void Initialize();

  void Draw();

  void AddElement(ZUIElement* element);
  void AddElements(std::initializer_list<ZUIElement*> elements);

  ZGraphicsStrategy* Strategy() { return graphicsStrategy_; }

  void EnableCursor();
  void DisableCursor();

  ZUICursor* Cursor() { return cursor_; }
  std::vector<ZUIElement*>& Elements() { return elements_; }

  void CleanUp();

protected:
  std::vector<ZUIElement*> elements_;
  ZUICursor* cursor_ = nullptr;
  ZGraphicsStrategy* graphicsStrategy_ = nullptr;
  ZShader* uiShader_ = nullptr;
};
