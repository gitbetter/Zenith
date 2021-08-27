/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourceLoader.hpp

    Created by Adrian Sanchez on 09/03/2019.
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

#include "ZCommon.hpp"

class ZIResourceLoader
{

public:

    virtual ~ZIResourceLoader() = default;
    virtual std::string Pattern() = 0;
    virtual bool UseRawFile() = 0;
    virtual unsigned int LoadedResourceSize(char* rawBuffer, unsigned int rawSize) = 0;
    virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, void* resource) = 0;

};

template <typename Data>
class ZResourceLoaderBase : public ZIResourceLoader
{

public:

    virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, void* resource) override
    {
        Data* castedResource = static_cast<Data*>(resource);
        assert(castedResource != nullptr && "Could not load resource using a resource loader with a mismatched resource type!");
        return Load(rawBuffer, rawSize, castedResource);
    }

protected:

    virtual bool Load(char* rawBuffer, unsigned int rawSize, Data* resource) = 0;

};
