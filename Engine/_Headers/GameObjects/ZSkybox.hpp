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

#include "ZGameObject.hpp"
#include "ZFramebuffer.hpp"

class ZTextureReadyEvent;
class ZRenderStateGroup;

struct ZSkybox : public ZGameObject
{


public:

    ZSkybox();
    ~ZSkybox() {}

    virtual void OnCreate() override;
    virtual void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) override;
    virtual void OnCloned(ZGameObject* original) override;

    void LoadCubemap(const std::string& hdr);

public:

    ZIBLTextureData iblTexture;

private:

    void LoadCubemap(const ZHTexture& cubeMap, const ZFramebuffer::ptr& bufferData);
    void HandleCubemapReady(const std::shared_ptr<ZTextureReadyEvent>& event);

private:

    std::string hdrPath_;

};
