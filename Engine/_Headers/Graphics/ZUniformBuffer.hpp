/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUniformBuffer.hpp

    Created by Adrian Sanchez on 03/01/2021.
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

enum class ZUniformBufferType
{
    None = 0, Camera, Object, Model, Material, Light, UI, Post, Last, UserDefined = Last
};

class ZUniformBuffer
{

public:

    using ptr = std::shared_ptr<ZUniformBuffer>;

    ZUniformBuffer(uint16_t index)
        : index_(index)
    { }
    virtual ~ZUniformBuffer() {}

    uint16_t Index() const { return index_; }

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Load(unsigned int size) = 0;
    virtual void Update(unsigned int offset, unsigned int size, const void* data) = 0;
    virtual void Delete() = 0;

    static ptr Create(uint16_t index, unsigned int size);
    static ptr Create(ZUniformBufferType type, unsigned int size);

protected:

    unsigned int ubo_ = 0;
    unsigned int size_ = 0;
    uint16_t index_;

};
