/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLFramebuffer.hpp

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

#include "ZGLFramebuffer.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void ZGLFramebuffer::LoadColor(const glm::vec2& size, const ZHTexture& colorTexture, bool multisample)
{  
    // This frame buffer has already been created, but we can still use this Load method to add texture attachments
    if (id_ > 0) {
        Delete();
    }

    size_ = size;
    multisampled_ = multisample;
    glGenFramebuffers(1, &id_);
    glBindFramebuffer(GL_FRAMEBUFFER, id_);

    if (!colorTexture.IsNull()) {
        attachments_.push_back(colorTexture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ZAssets::TextureManager()->IsMultisampled(colorTexture) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, ZAssets::TextureManager()->PlatformHandle(colorTexture), 0);
    }

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glGenRenderbuffers(1, &rboId_);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId_);

    if (multisampled_)
    {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, size.x, size.y);
    }
    else
    {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId_);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
    {
        LOG("Framebuffer operation incomplete dimensions", ZSeverity::Error);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLFramebuffer::LoadDepth(const glm::vec2& size, const ZHTexture& depthTexture)
{
    // This frame buffer has already been created, but we can still use this Load method to add texture attachments
    if (id_ > 0) {
        Delete();
    }

    size_ = size;
    glGenFramebuffers(1, &id_);
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (!depthTexture.IsNull()) {
        attachments_.push_back(depthTexture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ZAssets::TextureManager()->PlatformHandle(depthTexture), 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
    {
        LOG("Framebuffer operation incomplete dimensions", ZSeverity::Error);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLFramebuffer::LoadCubeMap()
{
    if (id_ > 0) {
        Delete();
    }

    size_ = glm::vec2(CUBE_MAP_SIZE, CUBE_MAP_SIZE);
    glGenFramebuffers(1, &id_);
    glGenRenderbuffers(1, &rboId_);

    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size_.x, size_.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId_);
}

void ZGLFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

void ZGLFramebuffer::BindAttachment(unsigned int attachmentIndex)
{
    if (attachmentIndex >= attachments_.size() || attachmentIndex < 0) {
        return;
    }
    if (attachmentIndex != boundAttachment_) {
        boundAttachment_ = attachmentIndex;
        ZHTexture texture = attachments_[boundAttachment_];
        GLenum textureTarget = ZAssets::TextureManager()->Type(texture) == "color" ? GL_COLOR_ATTACHMENT0 : GL_DEPTH_ATTACHMENT;
        // TODO: Do a better check for array textures or move this texture binding logic elsewhere!
        glFramebufferTexture2D(GL_FRAMEBUFFER, textureTarget, ZAssets::TextureManager()->IsMultisampled(texture) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, ZAssets::TextureManager()->PlatformHandle(texture), 0);
    }
}

void ZGLFramebuffer::BindAttachmentLayer(unsigned int layer) {
    ZHTexture texture = attachments_[boundAttachment_];
    std::string textureType = ZAssets::TextureManager()->Type(texture);
    if (textureType.find("Array") == std::string::npos)
        return;

    GLenum textureTarget = textureType == "colorArray" ? GL_COLOR_ATTACHMENT0 : GL_DEPTH_ATTACHMENT;
    // TODO: Do a better check for array textures or move this texture binding logic elsewhere!
    glFramebufferTextureLayer(GL_FRAMEBUFFER, textureTarget, ZAssets::TextureManager()->PlatformHandle(texture), 0, layer);
}

void ZGLFramebuffer::BindRenderbuffer()
{
    glBindRenderbuffer(GL_RENDERBUFFER, rboId_);
}

void ZGLFramebuffer::Resize(unsigned int width, unsigned int height)
{
    if ((size_.x == width && size_.y == height)) return;

    size_ = glm::vec2(width, height);
    for (const auto& attachment : attachments_) {
        ZAssets::TextureManager()->Resize(attachment, width, height);
    }

    BindRenderbuffer();

    if (multisampled_)
    {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    }
    else
    {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    }

    UnbindRenderbuffer();
}

void ZGLFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, size_.x, size_.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ZGLFramebuffer::UnbindRenderbuffer()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void ZGLFramebuffer::Blit(const ZFramebuffer::ptr& destination)
{
    glm::vec2 destinationSize = destination ? destination->Size() : size_;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination ? destination->ID() : 0);
    glBlitFramebuffer(0, 0, size_.x, size_.y, 0, 0, destinationSize.x, destinationSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLFramebuffer::Delete()
{
    if (id_ > 0)
    {
        for (const auto& attachment : attachments_)
        {
            ZAssets::TextureManager()->Delete(attachment);
        }
        glDeleteFramebuffers(1, &id_);
    }
}
