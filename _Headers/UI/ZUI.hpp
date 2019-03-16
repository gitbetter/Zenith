//
//  ZUI.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
// TODO: Conditional include based on graphics implementation
#include "ZGLGraphicsStrategy.hpp"
#include "ZUIElement.hpp"
#include "ZTextStrategy.hpp"
#include <vector>

// Forward Declarations
class ZUICursor;

// Class and Data Structure Definitions
class ZUI {
private:

public:

  ~ZUI() { }

  void Initialize();

  void Draw();

  void AddElement(std::shared_ptr<ZUIElement> element);
  void AddElements(std::initializer_list<std::shared_ptr<ZUIElement>> elements);

  void SetCursor(std::shared_ptr<ZUICursor> cursor) { cursor_ = cursor; }
  void RemoveCursor() { cursor_ = nullptr; }

  void RegisterFont(std::string fontPath);

  std::shared_ptr<ZUICursor> Cursor() { return cursor_; }
  ZUIElementMap& Elements() { return elements_; }

  ZTextStrategy* TextStrategy() { return textStrategy_.get(); }

  std::shared_ptr<ZShader> TextShader() { return textShader_; }
  std::shared_ptr<ZShader> UIShader() { return uiShader_; }

  template<class T> std::shared_ptr<T> FindElement(std::string id) {
    for (ZUIElementMap::iterator it = elements_.begin(); it != elements_.end(); it++) {
      if (std::dynamic_pointer_cast<T>(it->second) && it->second->ID() == id)
        return std::dynamic_pointer_cast<T>(it->second);
    }
    return nullptr;
  }

  void CleanUp();

protected:

  ZUIElementMap elements_;
  std::shared_ptr<ZUICursor> cursor_;
  std::unique_ptr<ZTextStrategy> textStrategy_;
  std::shared_ptr<ZShader> uiShader_;
  std::shared_ptr<ZShader> textShader_;
};
