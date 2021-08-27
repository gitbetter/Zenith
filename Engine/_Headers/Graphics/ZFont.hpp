/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZFont.hpp

    Created by Adrian Sanchez on 29/01/2021.
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

#pragma once

#include "ZTexture.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

class ZResourceLoadedEvent;

struct ZAtlas
{
    unsigned int width;
    unsigned int height;
    ZHTexture texture;
    std::map<unsigned char, ZCharacter> characterInfo;
};

class ZFont
{

public:

    using ptr = std::shared_ptr<ZFont>;

    const std::string& Name() const { return name_; }
    float Size() const { return size_; }
    ZAtlas Atlas() const { return atlas_; }
    ZCharacter Character(unsigned char c) { return atlas_.characterInfo[c]; }

    virtual void Load(const std::string& fontPath, unsigned int fontSize);
    virtual void LoadAsync(const std::string& fontPath, unsigned int fontSize);

    virtual void Load(class ZResourceData* resource, unsigned int fontSize) = 0;
    virtual void SetSize(unsigned int size) = 0;

    static void CreateAsync(std::shared_ptr<ZOFNode> data, ZFontIDMap& outPendingFonts);
    static void Create(std::shared_ptr<ZOFNode> data, ZFontMap& outFontMap);
    static ZFont::ptr Create(const std::string& fontPath, unsigned int fontSize);
    static ZFont::ptr CreateAsync(const std::string& fontPath, unsigned int fontSize);

protected:

    std::string name_;
    float size_;
    ZAtlas atlas_;

    // TODO: Add necessary synchronization primitives
    static FT_Library ft_;
    static bool initialized_;

    static void InitializeFreeType();
};
