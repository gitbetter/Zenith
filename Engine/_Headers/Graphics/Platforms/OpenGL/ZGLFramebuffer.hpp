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

#pragma once

// Includes
#include "ZFramebuffer.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZGLFramebuffer : public ZFramebuffer
{

public:

    void LoadColor(const glm::vec2& size, const ZTexture::ptr& colorTexture = nullptr, bool multisample = false) override;
    void LoadDepth(const glm::vec2& size, const ZTexture::ptr& depthTexture = nullptr) override;
    void LoadCubeMap() override;

    void Bind() override;
    void BindAttachment(unsigned int attachmentIndex = 0) override;
    void BindAttachmentLayer(unsigned int layer) override;
    void BindRenderbuffer() override;
    void Resize(unsigned int width, unsigned int height) override;
    void Unbind() override;
    void UnbindRenderbuffer() override;
    void Blit(const ZFramebuffer::ptr& destination = nullptr) override;
    void Delete() override;

};
