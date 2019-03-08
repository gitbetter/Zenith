//
//  ZTextStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

// Class and Data Structure Definitions

class ZTextStrategy {
private:

public:

  ZTextStrategy() { }
  virtual ~ZTextStrategy() { }

  virtual void Initialize();
  virtual void LoadFont(std::string fontPath, unsigned int fontSize = 24) = 0;
  virtual void SetFontSize(std::string font, unsigned int size);

  ZCharacter Character(std::string font, unsigned char c);

protected:

  FT_Library ft_;
  std::map<std::string, ZFont> loadedFonts_;
};
