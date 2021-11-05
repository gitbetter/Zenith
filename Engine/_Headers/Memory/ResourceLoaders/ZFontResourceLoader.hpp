/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZFontResourceLoader.hpp

    Created by Adrian Sanchez on 11/05/2021.
    Copyright © 2021 Pervasive Sense. All rights reserved.

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

#include "ZResourceLoader.hpp"
#include "ZResourceData.hpp"

class ZFontResourceLoader : public ZResourceLoaderBase<ZFontResourceData>
{

public:

    ~ZFontResourceLoader() {}
    std::string Pattern() override { return ".*\\.ttf|.*\\.otf|.*\\.eot|.*\\.woff|.*\\.woff2|.*\\.svg"; }
    bool UseRawFile() override { return true; };
    unsigned int LoadedResourceSize(char* rawBuffer, unsigned int rawSize) override { return rawSize; }

protected:

    bool Load(char* rawBuffer, unsigned int rawSize, ZFontResourceData* resource) override { return true; }

};
