//
//  ZUI.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
// TODO: Conditional include based on graphics implementation
#include "ZGLGraphicsStrategy.hpp"
#include "ZUIElement.hpp"
#include <vector>

// Forward Declarations
class ZUICursor;
class ZTextStrategy;

// Class and Data Structure Definitions
class ZUI {
private:

public:

  ~ZUI() { }

  void Initialize();

  void Draw();

  void AddElement(ZUIElement* element);
  void AddElements(std::initializer_list<ZUIElement*> elements);

  void EnableCursor();
  void DisableCursor();

  void RegisterFont(std::string fontPath);

  ZUICursor* Cursor() { return cursor_; }
  std::vector<ZUIElement*>& Elements() { return elements_; }

  ZGraphicsStrategy* GraphicsStrategy() { return graphicsStrategy_; }
  ZTextStrategy* TextStrategy() { return textStrategy_; }

  ZShader* TextShader() { return textShader_; }
  ZShader* UIShader() { return uiShader_; }

  void CleanUp();

protected:

  std::vector<ZUIElement*> elements_;
  ZUICursor* cursor_ = nullptr;
  ZGraphicsStrategy* graphicsStrategy_ = nullptr;
  ZTextStrategy* textStrategy_ = nullptr;
  ZShader* uiShader_ = nullptr;
  ZShader* textShader_ = nullptr;
};
