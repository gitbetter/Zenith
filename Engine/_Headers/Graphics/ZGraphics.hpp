/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZGraphics.hpp

 Created by Adrian Sanchez on 27/01/2019.
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
#include "ZEngine.hpp"
#include "ZTexture.hpp"

// Forward Declarations
class ZGameObject;
class ZWindow;
class ZShader;
class ZGraphicsStrategy;
class ZGraphicsDebug;
class ZLight;
class ZModel;
class ZMesh2D;
class ZScene;
class ZFrustum;
class ZResourceHandle;

// Class and Data Structure Definitions
typedef std::map<std::string, ZTexture> ZTextureMap;

class ZGraphics
{

private:

    std::map<std::string, ZTexture> pendingTextures_;
    std::map<std::shared_ptr<ZShader>, std::string> pendingShaders_;
    std::map<std::shared_ptr<ZModel>, std::string> pendingModels_;

    void InitializeBuffers();
    void InitializeShaders();

public:

    ZGraphics() : hasPBR_(true) {}
    ~ZGraphics() {}

    void Initialize();
    void Load(std::shared_ptr<ZOFTree> root);
    void LoadAsync(std::shared_ptr<ZOFTree> root);
    void CleanUp();

    void SetupShadowDepthPass(std::shared_ptr<ZLight> light, const ZFrustum& frustum);
    void SetupDepthPass();
    void SetupColorPass();
    void PostProcessing(ZScene* scene);
    void FinishRenderPass();
    void UsePBRPipeline(bool pbr = true) { hasPBR_ = pbr; }
    void UseMotionBlur(bool blur = false) { hasMotionBlur_ = blur; }
    bool HasPBR() const { return hasPBR_; }
    bool HasMotionBlur() const { return hasMotionBlur_; }

    ZGraphicsDebug* DebugDrawer() { return debugDrawer_.get(); }
    glm::mat4 LightSpaceMatrix() { return currentLightSpaceMatrix_; }
    ZTexture ShadowBuffer() { return shadowBuffer_; }
    ZTexture DepthBuffer() { return depthBuffer_; }
    ZTexture ColorBuffer() { return colorBuffer_; }
    ZTexture SceneBuffer() { return postBuffer_; }
    std::shared_ptr<ZShader> ShadowShader() { return shadowShader_; }
    std::shared_ptr<ZShader> DepthShader() { return depthShader_; }
    ZTextureMap& Textures() { return loadedTextures_; }
    ZShaderMap& Shaders() { return loadedShaders_; }
    ZModelMap& Models() { return loadedModels_; }

    void AddShader(const std::string& id, std::shared_ptr<ZShader> shader);
    void AddTexture(const std::string& id, const ZTexture& texture);
    void AddModel(const std::string& id, std::shared_ptr<ZModel> model);

    void HandleShaderReady(const std::shared_ptr<ZEvent>& event);
    void HandleTextureReady(const std::shared_ptr<ZEvent>& event);
    void HandleModelReady(const std::shared_ptr<ZEvent>& event);

    void ClearViewport();
    void SwapBuffers();
    void EnableStencilTesting();
    void EnableDepthTesting();
    void DisableDepthTesting();
    void EnableStencilBuffer();
    void DisableStencilBuffer();
    void EnableAlphaBlending();
    void DisableAlphaBlending();
    void EnableFaceCulling();
    void DisableFaceCulling();
    void EnableSeamlessCubemap();
    void DisableSeamlessCubemap();
    void EnableMSAA();
    void DisableMSAA();
    void CullFrontFaces();
    void CullBackFaces();
    void ClearDepth();
    void BindFramebuffer(const ZBufferData& frameBuffer, bool depth = false);
    void UnbindFramebuffer();
    void BlitFramebuffer(const ZBufferData& source, const ZBufferData& destination);
    ZTexture LoadDefaultTexture();
    void LoadTextureAsync(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true, bool equirect = false);
    ZTexture LoadTexture(const std::string& path, const std::string& directory, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true);
    ZTexture LoadTexture(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp, bool hdr = false, bool flip = true);
    ZTexture LoadEmptyLUT();
    ZTexture LoadColorTexture(bool multisample = false);
    ZTexture LoadDepthTexture();
    void BindTexture(const ZTexture& texture, unsigned int index);
    ZBufferData LoadColorBuffer(const ZTexture& colorTexture, bool multisample = false);
    ZBufferData LoadDepthMapBuffer(const ZTexture& depthTexture);
    ZBufferData LoadCubeMapBuffer();
    ZTexture LoadCubeMap(const std::vector<std::string>& faces);
    ZTexture LoadEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal);
    ZBufferData LoadVertexData(const ZVertex3DDataOptions& options);
    ZBufferData LoadVertexData(const ZVertex2DDataOptions& options);

    void ResizeColorTexture(const ZTexture& texture, unsigned int width, unsigned int height, bool multisample = false);
    void ResizeColorBuffer(const ZBufferData& bufferData, unsigned int width, unsigned int height, bool multisample = false);
    void DeleteBufferData(const ZBufferData& bufferData);

    void EquirectToCubemapAsync(const std::string& equirectHDRPath);
    ZTexture EquirectToCubemap(const std::string& equirectHDRPath, ZBufferData& bufferData);
    ZTexture EquirectToCubemap(const ZTexture& hdrTexture, ZBufferData& bufferData);

    ZTexture IrradianceMapFromCubeMap(const ZBufferData& cubemapBufferData, const ZTexture& cubemapTexture);
    ZTexture PrefilterCubeMap(const ZBufferData& cubemapBufferData, const ZTexture& cubemapTexture);
    ZTexture BRDFLUT(const ZBufferData& cubemapBufferData);

    void Draw(const ZBufferData& bufferData, const ZVertex3DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle);
    void Draw(const ZBufferData& bufferData, const ZVertex2DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::TriangleStrip);

    void UpdateBuffer(const ZBufferData& bufferData, const ZVertex2DDataOptions& vertexData);
    void UpdateBuffer(const ZBufferData& bufferData, const ZVertex3DDataOptions& vertexData);

    static void ComputeTangentBitangent(ZVertex3D& v1, ZVertex3D& v2, ZVertex3D& v3);

protected:

    std::shared_ptr<ZGraphicsStrategy> graphicsStrategy_ = nullptr;
    std::shared_ptr<ZGraphicsDebug> debugDrawer_ = nullptr;
    std::shared_ptr<ZShader> shadowShader_ = nullptr;
    std::shared_ptr<ZShader> depthShader_ = nullptr;
    std::shared_ptr<ZShader> postShader_ = nullptr;
    std::shared_ptr<ZMesh2D> renderQuad_ = nullptr;
    ZBufferData shadowFrameBuffer_, depthFrameBuffer_, colorFrameBufferMultisampled_;
    ZBufferData colorFrameBuffer_, postFrameBuffer_;
    ZTexture shadowBuffer_, depthBuffer_, colorBufferMultisampled_;
    ZTexture colorBuffer_, postBuffer_;
    glm::mat4 currentLightSpaceMatrix_;
    ZShaderMap loadedShaders_;
    ZTextureMap loadedTextures_;
    ZModelMap loadedModels_;
    bool hasPBR_ = true;
    bool hasMotionBlur_ = false;
};
