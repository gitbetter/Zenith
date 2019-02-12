//
//  ZTextStrategy.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZTextStrategy.hpp"

void ZTextStrategy::Initialize() {
  if (FT_Init_FreeType(&ft_)) _Z("Could not Initialize the FreeType library", ZERROR);
}

void ZTextStrategy::SetFontSize(std::string font, unsigned int fontSize) {
  if (loadedFonts_.find(font) == loadedFonts_.end()) {
    _Z("Can't set size on a font that has not been loaded yet", ZWARNING);
    return;
  }
  LoadFont(font, fontSize);
}

ZCharacter ZTextStrategy::Character(std::string font, unsigned char c) {
  if (loadedFonts_.find(font) == loadedFonts_.end()) {
    _Z("Could not get a character for that font. It seems it hasn't been loaded.", ZWARNING);
    return ZCharacter();
  }
  ZFont loadedFont = loadedFonts_[font];
  return loadedFont.characters[c];
}
