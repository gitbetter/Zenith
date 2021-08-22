/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZSkybox.hpp

 Created by Adrian Sanchez on 02/03/2019.
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
#include "ZGameObject.hpp"
#include "ZFramebuffer.hpp"

// Forward Declarations
class ZTexture;
class ZTextureReadyEvent;
class ZRenderStateGroup;

// Class and Data Structure Definitions
class ZSkybox : public ZGameObject
{

private:

    std::string hdrPath_;

    void Initialize(const ZTexture::ptr& cubeMap, const ZFramebuffer::ptr& bufferData);

public:

    ZSkybox(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
        : ZGameObject(position, orientation, scale) { }
    ZSkybox(const std::string& hdr = "");
    ~ZSkybox() {}

    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override;
    void InitializeAsync();

    std::shared_ptr<ZGameObject> Clone() override;

    bool IsVisible() override { return true; }

    ZIBLTextureData IBLTexture() const { return iblTexture_; }
    const std::shared_ptr<ZRenderStateGroup>& RenderState() const { return renderState_; }

    DECLARE_OBJECT_CREATORS(ZSkybox)

protected:

    std::shared_ptr<ZTexture> pendingHDRTexture_;
    ZIBLTextureData iblTexture_;

    std::shared_ptr<ZRenderStateGroup> renderState_;

    void HandleCubemapReady(const std::shared_ptr<ZTextureReadyEvent>& event);

};
