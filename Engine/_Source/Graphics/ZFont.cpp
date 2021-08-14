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

#include "ZGLFont.hpp"
#include "ZServices.hpp"
#include "ZResourceLoadedEvent.hpp"

FT_Library ZFont::ft_ = nullptr;
bool ZFont::initialized_ = false;

void ZFont::Load(const std::string& fontPath, unsigned int fontSize)
{
    InitializeFreeType();
    ZResource resource(fontPath, ZResourceType::Font);
    std::shared_ptr<ZResourceHandle> handle = ZServices::ResourceCache()->GetHandle(&resource);
    Load(handle, fontSize);
}

void ZFont::LoadAsync(const std::string& fontPath, unsigned int fontSize)
{
    InitializeFreeType();
    ZResource resource(fontPath, ZResourceType::Font);
    ZServices::ResourceCache()->GetHandleAsync(resource);
}

void ZFont::CreateAsync(std::shared_ptr<ZOFTree> data, ZFontIDMap& outPendingFonts)
{
}

void ZFont::Create(std::shared_ptr<ZOFTree> data, ZFontMap& outFontMap)
{
}


ZFont::ptr ZFont::Create(const std::string& fontPath, unsigned int fontSize)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZFont::ptr font = std::make_shared<ZGLFont>();
    font->Load(fontPath, fontSize);
    return font;
}

ZFont::ptr ZFont::CreateAsync(const std::string& fontPath, unsigned int fontSize)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZFont::ptr font = std::make_shared<ZGLFont>();
    font->LoadAsync(fontPath, fontSize);
    return font;
}

void ZFont::InitializeFreeType()
{
    if (!initialized_) {
        if (FT_Init_FreeType(&ft_)) {
            LOG("Could not Initialize the FreeType library", ZSeverity::Error);
            return;
        }
        initialized_ = true;
    }
}