/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGraphicsStrategy.hpp

    Created by Adrian Sanchez on 06/02/2019.
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
#include "ZMaterial.hpp"
#include "stb_image.hpp"

// Forward Declarations
class ZGameObject;
class ZResourceHandle;
class ZLight;

// Class and Data Structure Definitions
class ZGraphicsStrategy
{
private:

public:

    ZGraphicsStrategy() {}
    virtual ~ZGraphicsStrategy() {}

    virtual void Initialize() = 0;
    virtual void ClearViewport() = 0;
    virtual void SwapBuffers() = 0;
    virtual void EnableStencilTesting() = 0;
    virtual void EnableDepthTesting() = 0;
    virtual void DisableDepthTesting() = 0;
    virtual void EnableStencilBuffer() = 0;
    virtual void DisableStencilBuffer() = 0;
    virtual void EnableAlphaBlending() = 0;
    virtual void DisableAlphaBlending() = 0;
    virtual void EnableFaceCulling() = 0;
    virtual void DisableFaceCulling() = 0;
    virtual void EnableSeamlessCubemap() = 0;
    virtual void DisableSeamlessCubemap() = 0;
    virtual void EnableMSAA() = 0;
    virtual void DisableMSAA() = 0;
    virtual void CullFrontFaces() = 0;
    virtual void CullBackFaces() = 0;
    virtual void ClearDepth() = 0;
    virtual void BindFramebuffer(const ZBufferData& frameBuffer, bool depth = false) = 0;
    virtual void UnbindFramebuffer() = 0;
    virtual void BlitFramebuffer(const ZBufferData& source, const ZBufferData& destination) = 0;
    virtual ZTexture LoadDefaultTexture() = 0;
    virtual void LoadTextureAsync(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true, bool equirect = false) = 0;
    virtual ZTexture LoadTexture(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true) = 0;
    virtual ZTexture LoadTexture(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true) = 0;
    virtual ZTexture LoadEmptyLUT() = 0;
    virtual ZTexture LoadColorTexture(bool multisample = false) = 0;
    virtual ZTexture LoadDepthTexture() = 0;
    virtual void BindTexture(const ZTexture& texture, unsigned int index) = 0;
    virtual ZBufferData LoadColorBuffer(const ZTexture& colorTexture, bool multisample = false) = 0;
    virtual ZBufferData LoadDepthMapBuffer(const ZTexture& depthTexture) = 0;
    virtual ZBufferData LoadCubeMapBuffer() = 0;
    virtual ZTexture LoadCubeMap(const std::vector<std::string>& faces) = 0;
    virtual ZTexture LoadEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal) = 0;
    virtual ZBufferData LoadVertexData(const ZVertex3DDataOptions& options) = 0;
    virtual ZBufferData LoadVertexData(const ZVertex2DDataOptions& options) = 0;
    virtual void ResizeColorTexture(const ZTexture& texture, unsigned int width, unsigned int height, bool multisample = false) = 0;
    virtual void ResizeColorBuffer(const ZBufferData& bufferData, unsigned int width, unsigned int height, bool multisample = false) = 0;
    virtual void DeleteBufferData(const ZBufferData& bufferData) = 0;
    virtual void EquirectToCubemapAsync(const std::string& equirectHDRPath) = 0;
    virtual ZTexture EquirectToCubemap(const std::string& equirectHDRPath, ZBufferData& bufferData) = 0;
    virtual ZTexture EquirectToCubemap(const ZTexture& hdrTexture, ZBufferData& bufferData) = 0;
    virtual ZTexture IrradianceMapFromCubeMap(const ZBufferData& cubemapBufferData, const ZTexture& cubemapTexture) = 0;
    virtual ZTexture PrefilterCubeMap(const ZBufferData& cubemapBufferData, const ZTexture& cubemapTexture) = 0;
    virtual ZTexture BRDFLUT(const ZBufferData& cubemapBufferData) = 0;
    virtual void Draw(const ZBufferData& bufferData, const ZVertex3DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle) = 0;
    virtual void Draw(const ZBufferData& bufferData, const ZVertex2DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::TriangleStrip) = 0;
    virtual void UpdateBuffer(const ZBufferData& bufferData, const ZVertex2DDataOptions& vertexData) = 0;
    virtual void UpdateBuffer(const ZBufferData& bufferData, const ZVertex3DDataOptions& vertexData) = 0;

};
