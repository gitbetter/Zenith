/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZBuffer.hpp

    Created by Adrian Sanchez on 27/01/2021.
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

// Includes
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZBuffer
{

public:

    using ptr = std::shared_ptr<ZBuffer>;

    ZBuffer() {}
    virtual ~ZBuffer() {}

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Load(const ZVertex2DDataOptions& vertexData) = 0;
    virtual void Load(const ZVertex3DDataOptions& vertexData) = 0;
    virtual void Update(const ZVertex2DDataOptions& vertexData) = 0;
    virtual void Update(const ZVertex3DDataOptions& vertexData) = 0;
    virtual void Delete() = 0;

    static ptr Create(const ZVertex3DDataOptions& options);
    static ptr Create(const ZVertex2DDataOptions& options);

protected:

    unsigned int vbo_ = 0;
    unsigned int ivbo_ = 0;
    unsigned int ebo_ = 0;
    unsigned int vao_ = 0;

};
