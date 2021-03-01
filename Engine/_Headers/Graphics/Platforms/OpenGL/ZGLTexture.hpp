/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLTexture.hpp

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
#include "ZTexture.hpp"

// Forward Declarations
class ZResourceLoadedEvent;

// Class and Data Structure Definitions
class ZGLTexture : public ZTexture
{

public:

    ZGLTexture() : ZTexture()
    { }

    void Bind(unsigned int index) override;
    void Unbind() override;
    void Resize(unsigned int width, unsigned int height) override;
    void Delete() override;

    void LoadDefault() override;
    void LoadCubeMap(const std::vector<std::string>& faces) override;
    void LoadCubeMap(const ZTexture::ptr& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData) override;
    void LoadEmptyCubeMap(ZCubemapTextureType textureType = ZCubemapTextureType::Normal) override;
    void LoadHDRIAsync(const std::string& hdriPath) override;
    void LoadHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData) override;
    void LoadIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture) override;
    void LoadPrefilterCubeMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture) override;
    void LoadBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData) override;
    void Load(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true)override;
    void LoadEmptyLUT() override;
    void LoadColor(const glm::vec2& size, bool multisample = false) override;
    void LoadDepth(const glm::vec2& size) override;
    void LoadDepthArray(const glm::vec2& size, int layers) override;

protected:

    unsigned int index_ = 0;

};
