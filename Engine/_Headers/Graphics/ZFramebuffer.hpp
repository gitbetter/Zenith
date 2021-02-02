/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZFramebuffer.hpp

    Created by Adrian Sanchez on 28/01/2021.
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
#include "ZTexture.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZFramebuffer
{

protected:

    glm::vec2 size_;
    unsigned int id_;
    unsigned int rboId_;
    ZTexture::ptr attachment_;

public:

    using ptr = std::shared_ptr<ZFramebuffer>;

    ZFramebuffer() = default;
    virtual ~ZFramebuffer() { }

    unsigned int ID() const { return id_; }
    unsigned int RBID() const { return rboId_; }
    glm::vec2 Size() const { return size_; }
    ZTexture::ptr Attachment() { return attachment_; }

    virtual void LoadColor(const glm::vec2& size, const ZTexture::ptr& colorTexture, bool multisample = false) = 0;
    virtual void LoadDepth(const glm::vec2& size, const ZTexture::ptr& depthTexture) = 0;
    virtual void LoadCubeMap() = 0;

    virtual void Bind() = 0;
    virtual void BindRenderbuffer() = 0;
    virtual void Resize(unsigned int width, unsigned int height, bool multisample = false) = 0;
    virtual void Unbind() = 0;
    virtual void UnbindRenderbuffer() = 0;
    virtual void Blit(const ZFramebuffer::ptr& destination = nullptr) = 0;

    static ZFramebuffer::ptr CreateColor(const glm::vec2& size, bool multisample = false);
    static ZFramebuffer::ptr CreateDepth(const glm::vec2& size);
    static ZFramebuffer::ptr CreateCubeMap();

};
