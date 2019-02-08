//
//  ZGraphicsStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphicsCommon.hpp"
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
  virtual void EnableStencilBuffer() = 0;
  virtual void DisableStencilBuffer() = 0;
  virtual void BindFramebuffer(unsigned int frameBuffer) = 0;
  virtual void UnbindFramebuffer() = 0;
  virtual ZTexture LoadTexture(std::string path, const std::string &directory) = 0;
  virtual void BindTexture(ZTexture texture, unsigned int index) = 0;
  virtual ZTexture LoadDepthTexture() = 0;
  virtual unsigned int LoadDepthMapBuffer(ZTexture depthTexture) = 0;
  virtual void BindDepthMapBuffer(unsigned int frameBuffer) = 0;
  virtual void UnbindDepthMapBuffer() = 0;
  virtual unsigned int LoadCubeMap(std::vector<std::string> faces) = 0;
  virtual ZBufferData LoadVertexData(std::vector<ZVertex> vertices, std::vector<unsigned int> indices) = 0;
  virtual ZBufferData LoadVertexData(std::vector<glm::vec4> vertices) = 0;
  virtual void Draw(ZBufferData bufferData, std::vector<ZVertex> vertices, std::vector<unsigned int> indices) = 0;
  virtual void CleanUp() = 0;

protected:

};
