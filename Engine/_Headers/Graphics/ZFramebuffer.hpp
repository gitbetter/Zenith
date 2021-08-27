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

#include "ZCommon.hpp"
#include "ZTexture.hpp"

class ZFramebuffer
{

protected:

    glm::vec2 size_;
    unsigned int id_ = 0;
    unsigned int rboId_ = 0;
    unsigned int boundAttachment_ = 0;
    std::vector<ZHTexture> attachments_;
    bool multisampled_ = false;

public:

    using ptr = std::shared_ptr<ZFramebuffer>;

    ZFramebuffer() = default;
    virtual ~ZFramebuffer() { }

    unsigned int ID() const { return id_; }
    unsigned int RBID() const { return rboId_; }
    glm::vec2 Size() const { return size_; }
    ZHTexture BoundAttachment() const { return attachments_[boundAttachment_]; }
    std::vector<ZHTexture> Attachments() const { return attachments_; }

    void AddAttachment(const ZHTexture& attachment) { attachments_.push_back(attachment); }

    virtual void LoadColor(const glm::vec2& size, const ZHTexture& colorTexture = ZHTexture(), bool multisample = false) = 0;
    virtual void LoadDepth(const glm::vec2& size, const ZHTexture& depthTexture = ZHTexture()) = 0;
    virtual void LoadCubeMap() = 0;

    virtual void Bind() = 0;
    virtual void BindAttachment(unsigned int attachmentIndex = 0) = 0;
    virtual void BindAttachmentLayer(unsigned int layer) = 0;
    virtual void BindRenderbuffer() = 0;
    virtual void Resize(unsigned int width, unsigned int height) = 0;
    virtual void Unbind() = 0;
    virtual void UnbindRenderbuffer() = 0;
    virtual void Blit(const ZFramebuffer::ptr& destination = nullptr) = 0;
    virtual void Delete() = 0;

    static ZFramebuffer::ptr CreateColor(const glm::vec2& size, bool multisample = false);
    static ZFramebuffer::ptr CreateDepth(const glm::vec2& size);
    static ZFramebuffer::ptr CreateShadow(const glm::vec2& size, unsigned int cascades = 1);
    static ZFramebuffer::ptr CreateCubeMap();

};
