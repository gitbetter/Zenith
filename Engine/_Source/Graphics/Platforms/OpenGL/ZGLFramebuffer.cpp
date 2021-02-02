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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void ZGLFramebuffer::LoadColor(const glm::vec2& size, const ZTexture::ptr& colorTexture, bool multisample)
{
    attachment_ = colorTexture;
    size_ = size;
    glGenFramebuffers(1, &id_);
    glBindFramebuffer(GL_FRAMEBUFFER, id_);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, colorTexture->id, 0);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glGenRenderbuffers(1, &rboId_);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId_);

    if (multisample)
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

void ZGLFramebuffer::LoadDepth(const glm::vec2& size, const ZTexture::ptr& depthTexture)
{
    attachment_ = depthTexture;
    size_ = size;
    glGenFramebuffers(1, &id_);
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture->id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
    {
        LOG("Framebuffer operation incomplete dimensions", ZSeverity::Error);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLFramebuffer::LoadCubeMap()
{
    size_ = glm::vec2(CUBE_MAP_SIZE, CUBE_MAP_SIZE);
    glGenFramebuffers(1, &id_);
    glGenRenderbuffers(1, &rboId_);

    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBE_MAP_SIZE, CUBE_MAP_SIZE);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId_);
}

void ZGLFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
}

void ZGLFramebuffer::BindRenderbuffer()
{
    glBindRenderbuffer(GL_RENDERBUFFER, rboId_);
}

void ZGLFramebuffer::Resize(unsigned int width, unsigned int height, bool multisample)
{
    if ((size_.x == width && size_.y == height)) return;

    if (attachment_) {
        size_ = glm::vec2(width, height);
        attachment_->Resize(width, height, multisample);
    }

    BindRenderbuffer();

    if (multisample)
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