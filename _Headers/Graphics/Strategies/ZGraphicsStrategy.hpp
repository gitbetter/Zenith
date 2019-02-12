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
  virtual void BindFramebuffer(unsigned int frameBuffer) = 0;
  virtual void UnbindFramebuffer() = 0;
  virtual ZTexture LoadDefaultTexture() = 0;
  virtual ZTexture LoadTexture(std::string path, const std::string &directory) = 0;
  virtual void BindTexture(ZTexture texture, unsigned int index) = 0;
  virtual ZTexture LoadDepthTexture() = 0;
  virtual unsigned int LoadDepthMapBuffer(ZTexture depthTexture) = 0;
  virtual void BindDepthMapBuffer(unsigned int frameBuffer) = 0;
  virtual void UnbindDepthMapBuffer() = 0;
  virtual unsigned int LoadCubeMap(std::vector<std::string> faces) = 0;
  virtual ZBufferData LoadVertexData(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) = 0;
  virtual ZBufferData LoadVertexData(std::vector<ZVertex2D> vertices) = 0;
  virtual ZBufferData LoadEmptyVertexData2D(unsigned int size) = 0;
  virtual void UpdateBuffer(ZBufferData buffer, std::vector<ZVertex2D> data) = 0;
  virtual void Draw(ZBufferData bufferData, std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) = 0;
  virtual void Draw(ZBufferData bufferData, std::vector<ZVertex2D> vertices) = 0;

protected:

};
