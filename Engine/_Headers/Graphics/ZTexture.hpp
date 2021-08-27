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

#include "ZCommon.hpp"
#include "ZOFTree.hpp"
#include "ZHandle.hpp"
#include "ZResourcePool.hpp"

class ZFramebuffer;
class ZResourceLoadedEvent;
struct ZIBLTextureData;

struct ZTexture
{
	unsigned int id{ 0 };
	unsigned int index{ 0 };
	ZTextureWrapping wrapping{ ZTextureWrapping::Repeat };
	std::string name;
	std::string type;
	std::string path;
	bool multisampled{ false };

	ZTexture();

private:

	static ZIDSequence idGenerator_;

};

struct ZIBLTextureData
{
	ZHTexture cubeMap;
	ZHTexture irradiance;
	ZHTexture prefiltered;
	ZHTexture brdfLUT;
};

class ZTextureManager
{

    using ZTexturePool = ZResourcePool<ZTexture, ZHTexture>;

public:

    ZTextureManager();
    virtual ~ZTextureManager() = default;

    ZHTexture Default();

    ZHTexture Create();
    void Create(std::shared_ptr<ZOFNode> data, ZTextureMap& outTextureMap);
	ZHTexture Create(const std::string& path, const std::string& type = "", ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true);
	void CreateAsync(std::shared_ptr<ZOFNode> data);
    void CreateAsync(const std::string& path, const std::string& type = "", ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true, bool equirect = false);
    ZIBLTextureData CreateIBL(const std::shared_ptr<ZFramebuffer>& bufferData, const ZHTexture& cubemap);

	bool IsLoaded(const std::string& name);
	ZHTexture GetFromName(const std::string& name);

	unsigned int PlatformHandle(const ZHTexture& handle);
	std::string Name(const ZHTexture& handle);
	std::string Type(const ZHTexture& handle);
	std::string Path(const ZHTexture& handle);
	bool IsMultisampled(const ZHTexture& handle);
	ZTextureWrapping Wrapping(const ZHTexture& handle);

	void SetPlatformHandle(const ZHTexture& handle, unsigned int pHandle);
	void SetName(const ZHTexture& handle, const std::string& name);
	void SetType(const ZHTexture& handle, const std::string& type);
	void SetPath(const ZHTexture& handle, const std::string& path);
	void SetIsMultisampled(const ZHTexture& handle, bool multisampled);
	void SetWrapping(const ZHTexture& handle, ZTextureWrapping wrapping);

public:

    virtual void Initialize();
	virtual void CleanUp();

	virtual ZHTexture Create(class ZTextureResourceData* resource, const std::string& type = "", ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true) = 0;
	virtual ZHTexture CreateDefault() = 0;
	virtual ZHTexture CreateEmptyLUT() = 0;
	virtual ZHTexture CreateColor(const glm::vec2& size, bool multisample = false) = 0;
	virtual ZHTexture CreateDepth(const glm::vec2& size) = 0;
	virtual ZHTexture CreateDepthArray(const glm::vec2& size, int layers) = 0;
	virtual ZHTexture CreateCubeMap(const std::vector<std::string>& faces) = 0;
	virtual ZHTexture CreateCubeMap(const ZHTexture& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData) = 0;
	virtual ZHTexture CreateEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal) = 0;
	virtual ZHTexture CreateHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData) = 0;
	virtual void CreateHDRIAsync(const std::string& hdriPath) = 0;
	virtual ZHTexture CreateIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZHTexture& cubemapTexture) = 0;
	virtual ZHTexture CreatePrefilterMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZHTexture& cubemapTexture) = 0;
	virtual ZHTexture CreateBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData) = 0;

    virtual void Resize(const ZHTexture& handle, unsigned int width, unsigned int height) = 0;
    virtual void Bind(const ZHTexture& handle, unsigned int index) = 0;
    virtual void Unbind(const ZHTexture& handle) = 0;
    virtual void Delete(const ZHTexture& handle) = 0;


protected:

    ZTexturePool texturePool_;
	ZTextureMap loadedTextures_;

    std::map<std::string, ZTextureWrapping> pendingTextureWrappings_;
	std::map<std::string, std::string> pendingTextureTypes_;

protected:

	/** Adds a texture to the internal loaded texture map so that we don't accidentally recreate duplicates of the texture */
	void TrackTexture(const ZHTexture& handle);

private:

    void HandleTextureLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event);

};