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

#include "ZTexture.hpp"

class ZGLTextureManager : public ZTextureManager
{

public:

    ZGLTextureManager() : ZTextureManager()
    { }

	ZHTexture Create(ZTextureResourceData* resource, const std::string& type = "", ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true, const ZHTexture& restoreHandle = ZHTexture()) override;
	ZHTexture CreateDefault() override;
	ZHTexture CreateEmptyLUT() override;
	ZHTexture CreateColor(const glm::vec2& size, bool multisample = false) override;
	ZHTexture CreateDepth(const glm::vec2& size) override;
	ZHTexture CreateDepthArray(const glm::vec2& size, int layers) override;
	ZHTexture CreateCubeMap(const std::vector<std::string>& faces) override;
	ZHTexture CreateCubeMap(const ZHTexture& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData) override;
	ZHTexture CreateEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal) override;
	ZHTexture CreateHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData) override;
	void CreateHDRIAsync(const std::string& hdriPath) override;
	ZHTexture CreateIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZHTexture& cubemapTexture) override;
	ZHTexture CreatePrefilterMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZHTexture& cubemapTexture) override;
	ZHTexture CreateBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData) override;

	void Resize(const ZHTexture& handle, unsigned int width, unsigned int height) override;
	void Bind(const ZHTexture& handle, unsigned int index) override;
	void Unbind(const ZHTexture& handle) override;
	void Delete(const ZHTexture& handle) override;

};
