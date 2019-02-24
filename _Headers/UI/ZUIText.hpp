//
//  ZUIText.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIElement.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZUIText : public ZUIElement {
private:

public:

  ZUIText(std::string text, std::string font = "", float fontSize = 1.f, glm::vec2 position = glm::vec2(0.5f), glm::vec2 scale = glm::vec2(0.1f));
  ~ZUIText() { }

  void Draw(ZShader* shader) override;

protected:

  std::string text_;
  std::string font_;
  float fontScale_;
  ZBufferData bufferData_;
};
