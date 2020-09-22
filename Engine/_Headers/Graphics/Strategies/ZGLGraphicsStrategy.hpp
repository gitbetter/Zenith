/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLGraphicsStrategy.hpp

    Created by Adrian Sanchez on 07/02/2019.
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
#include "ZGraphicsStrategy.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions

class ZGLGraphicsStrategy : public ZGraphicsStrategy
{

private:

    static void GLFWErrorCallback(int id, const char* description);

public:

    ZGLGraphicsStrategy() {}
    ~ZGLGraphicsStrategy();

    void Initialize() override;
    void ClearViewport() override;
    void SwapBuffers() override;
    void EnableStencilTesting() override;
    void EnableDepthTesting() override;
    void DisableDepthTesting() override;
    void EnableStencilBuffer() override;
    void DisableStencilBuffer() override;
    void EnableAlphaBlending() override;
    void DisableAlphaBlending() override;
    void EnableFaceCulling() override;
    void DisableFaceCulling() override;
    void EnableSeamlessCubemap() override;
    void DisableSeamlessCubemap() override;
    void EnableMSAA() override;
    void DisableMSAA() override;
    void CullFrontFaces() override;
    void CullBackFaces() override;
    void ClearDepth() override;
    void BindFramebuffer(const ZBufferData& frameBuffer, bool depth = false) override;
    void UnbindFramebuffer() override;
    void BlitFramebuffer(const ZBufferData& source, const ZBufferData& destination) override;
    ZTexture LoadDefaultTexture() override;
    void LoadTextureAsync(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true, bool equirect = false) override;
    ZTexture LoadTexture(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true) override;
    ZTexture LoadTexture(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true) override;
    ZTexture LoadEmptyLUT() override;
    ZTexture LoadColorTexture(bool multisample = false) override;
    ZTexture LoadDepthTexture() override;
    void BindTexture(const ZTexture& texture, unsigned int index) override;
    ZBufferData LoadColorBuffer(const ZTexture& colorTexture, bool multisample = false) override;
    ZBufferData LoadDepthMapBuffer(const ZTexture& depthTexture) override;
    ZBufferData LoadCubeMapBuffer() override;
    ZTexture LoadCubeMap(const std::vector<std::string>& faces) override;
    ZTexture LoadEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal) override;
    ZBufferData LoadVertexData(const ZVertex3DDataOptions& options) override;
    ZBufferData LoadVertexData(const ZVertex2DDataOptions& options) override;

    void ResizeColorTexture(const ZTexture& texture, unsigned int width, unsigned int height, bool multisample = false) override;
    void ResizeColorBuffer(const ZBufferData& bufferData, unsigned int width, unsigned int height, bool multisample = false) override;
    void DeleteBufferData(const ZBufferData& bufferData) override;

    void EquirectToCubemapAsync(const std::string& equirectHDRPath) override;
    ZTexture EquirectToCubemap(const std::string& equirectHDRPath, ZBufferData& bufferData) override;
    ZTexture EquirectToCubemap(const ZTexture& hdrTexture, ZBufferData& bufferData) override;

    ZTexture IrradianceMapFromCubeMap(const ZBufferData& cubemapBufferData, const ZTexture& cubemapTexture) override;
    ZTexture PrefilterCubeMap(const ZBufferData& cubemapBufferData, const ZTexture& cubemapTexture) override;
    ZTexture BRDFLUT(const ZBufferData& cubemapBufferData) override;

    void Draw(const ZBufferData& bufferData, const ZVertex3DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle) override;
    void Draw(const ZBufferData& bufferData, const ZVertex2DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::TriangleStrip) override;

    void UpdateBuffer(const ZBufferData& bufferData, const ZVertex2DDataOptions& vertexData) override;
    void UpdateBuffer(const ZBufferData& bufferData, const ZVertex3DDataOptions& vertexData) override;

protected:

    std::map<std::string, ZTextureWrapping> pendingTextureWrappings_;
    std::map<ZMeshDrawStyle, unsigned int> drawingStylesMap_;

    void HandleTextureLoaded(const std::shared_ptr<ZEvent>& event);

};
