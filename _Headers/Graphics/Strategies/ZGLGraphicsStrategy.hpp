//
//  ZGLGraphicsStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 07/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGraphicsStrategy.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions

class ZGLGraphicsStrategy : public ZGraphicsStrategy {
private:

  static void GLFWErrorCallback(int id, const char* description);

public:

  ZGLGraphicsStrategy() { }
  ~ZGLGraphicsStrategy() { }

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
  void BindFramebuffer(ZBufferData frameBuffer) override;
  void UnbindFramebuffer() override;
  ZTexture LoadDefaultTexture() override;
  ZTexture LoadTexture(std::string path, const std::string &directory, bool hdr = false, bool flip = false) override;
  void BindTexture(ZTexture texture, unsigned int index) override;
  ZTexture LoadDepthTexture() override;
  ZBufferData LoadDepthMapBuffer(ZTexture depthTexture) override;
  ZBufferData LoadCubeMapBuffer() override;
  void BindDepthMapBuffer(ZBufferData frameBuffer) override;
  void UnbindDepthMapBuffer() override;
  void BindCubeMapBuffer(ZBufferData cubemapBuffer) override;
  ZTexture LoadCubeMap(std::vector<std::string> faces) override;
  ZTexture LoadEmptyCubeMap(bool irradiance = false) override;
  ZBufferData LoadIndexedVertexData(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) override;
  ZBufferData LoadVertexData(std::vector<ZVertex3D> vertices) override;
  ZBufferData LoadVertexData(std::vector<ZVertex2D> vertices) override;
  ZBufferData LoadEmptyVertexData2D(unsigned int size) override;
  void DeleteBufferData(ZBufferData bufferData) override;
  void UpdateBuffer(ZBufferData buffer, std::vector<ZVertex2D> data) override;

  ZTexture EquirectToCubemap(std::string equirectHDRPath, ZBufferData& bufferData) override;
  ZTexture IrradianceMapFromCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) override;

  void Draw(ZBufferData bufferData, std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) override;
  void Draw(ZBufferData bufferData, std::vector<ZVertex2D> vertices) override;
  void DrawLines(ZBufferData bufferData, std::vector<ZVertex3D> vertices) override;

protected:


};
