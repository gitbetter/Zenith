/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZTextStrategy.cpp

    Created by Adrian Sanchez on 11/02/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

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
    _Z("The font " + font + " has not been loaded.", ZWARNING);
    return ZCharacter();
  }
  ZFont loadedFont = loadedFonts_[font];
  return loadedFont.characters[c];
}
