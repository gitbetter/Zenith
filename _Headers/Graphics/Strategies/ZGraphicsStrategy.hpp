//
//  ZGraphicsStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZMaterial.hpp"
#include "stb_image.hpp"

// Forward Declarations
class ZGameObject;
struct ZLight;

// Class and Data Structure Definitions
class ZGraphicsStrategy {
private:

public:

  ZGraphicsStrategy() { }
  virtual ~ZGraphicsStrategy() { }

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
  virtual void BindFramebuffer(ZBufferData frameBuffer) = 0;
  virtual void UnbindFramebuffer() = 0;
  virtual ZTexture LoadDefaultTexture() = 0;
  virtual ZTexture LoadTexture(std::string path, const std::string &directory, bool hdr = false, bool flip = false) = 0;
  virtual ZTexture LoadEmptyLUT() = 0;
  virtual void BindTexture(ZTexture texture, unsigned int index) = 0;
  virtual ZTexture LoadDepthTexture() = 0;
  virtual ZBufferData LoadDepthMapBuffer(ZTexture depthTexture) = 0;
  virtual ZBufferData LoadCubeMapBuffer() = 0;
  virtual void BindDepthMapBuffer(ZBufferData frameBuffer) = 0;
  virtual void UnbindDepthMapBuffer() = 0;
  virtual void BindCubeMapBuffer(ZBufferData cubemapBuffer) = 0;
  virtual ZTexture LoadCubeMap(std::vector<std::string> faces) = 0;
  virtual ZTexture LoadEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal) = 0;
  virtual ZBufferData LoadIndexedVertexData(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) = 0;
  virtual ZBufferData LoadVertexData(std::vector<ZVertex3D> vertices) = 0;
  virtual ZBufferData LoadVertexData(std::vector<ZVertex2D> vertices) = 0;
  virtual ZBufferData LoadEmptyVertexData2D(unsigned int size) = 0;
  virtual void DeleteBufferData(ZBufferData bufferData) = 0;
  virtual void UpdateBuffer(ZBufferData buffer, std::vector<ZVertex2D> data) = 0;
  virtual ZTexture EquirectToCubemap(std::string equirectHDRPath, ZBufferData& bufferData) = 0;
  virtual ZTexture IrradianceMapFromCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) = 0;
  virtual ZTexture PrefilterCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) = 0;
  virtual ZTexture BRDFLUT(ZBufferData cubemapBufferData) = 0;
  virtual void Draw(ZBufferData bufferData, std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle) = 0;
  virtual void Draw(ZBufferData bufferData, std::vector<ZVertex2D> vertices, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::TriangleStrip) = 0;
  virtual void DrawLines(ZBufferData bufferData, std::vector<ZVertex3D> vertices) = 0;

protected:

};
