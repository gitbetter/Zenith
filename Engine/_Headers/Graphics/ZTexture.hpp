/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZTexture.hpp

    Created by Adrian Sanchez on 24/01/2021.
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
class ZResourceHandle;
class ZFramebuffer;
class ZResourceLoadedEvent;

// Class and Data Structure Definitions
class ZTexture : public std::enable_shared_from_this<ZTexture>
{

public:

    using ptr = std::shared_ptr<ZTexture>;

    unsigned int id;
    std::string name;
    std::string type;
    std::string path;
    ZTextureWrapping wrapping;

    ZTexture();
    virtual ~ZTexture() = default;

    virtual void LoadAsync(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true, bool equirect = false);
    virtual void Load(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true);

    virtual void Resize(unsigned int width, unsigned int height, bool multisample = false) = 0;
    virtual void Bind(unsigned int index) = 0;

    virtual void LoadDefault() = 0;
    virtual void LoadCubeMap(const std::vector<std::string>& faces) = 0;
    virtual void LoadCubeMap(const ZTexture::ptr& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData) = 0;
    virtual void LoadEmptyCubeMap(ZCubemapTextureType textureType = ZCubemapTextureType::Normal) = 0;
    virtual void LoadHDRIAsync(const std::string& hdriPath) = 0;
    virtual void LoadHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData) = 0;
    virtual void LoadIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture) = 0;
    virtual void LoadPrefilterCubeMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture) = 0;
    virtual void LoadBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData) = 0;
    virtual void Load(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true) = 0;
    virtual void LoadEmptyLUT() = 0;
    virtual void LoadColor(const glm::vec2& size, bool multisample = false) = 0;
    virtual void LoadDepth(const glm::vec2& size) = 0;

    static void Create(std::shared_ptr<ZOFTree> data, ZTextureMap& outTextureMap);
    static void CreateAsync(std::shared_ptr<ZOFTree> data, ZTextureIDMap& outPendingTextures);
    static ptr Create(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true);
    static ptr Create(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true);
    static ptr CreateAsync(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true, bool equirect = false);
    static ptr CreateDefault();
    static ptr CreateEmptyLUT();
    static ptr CreateColor(const glm::vec2& size, bool multisample = false);
    static ptr CreateDepth(const glm::vec2& size);
    static ptr CreateCubeMap(const std::vector<std::string>& faces);
    static ptr CreateCubeMap(const ZTexture::ptr& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData);
    static ptr CreateEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal);
    static ptr CreateHDRIAsync(const std::string& hdriPath);
    static ptr CreateHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData);
    static ptr CreateIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture);
    static ptr CreatePrefilterMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture);
    static ptr CreateBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData);

private:

    static std::map<std::string, ZTextureWrapping> pendingTextureWrappings_;

    void HandleTextureLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);

};

struct ZIBLTexture
{
    ZTexture::ptr cubeMap;
    ZTexture::ptr irradiance;
    ZTexture::ptr prefiltered;
    ZTexture::ptr brdfLUT;
};