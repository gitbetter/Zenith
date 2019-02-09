//
//  ZGLGraphicsStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 07/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphicsCommon.hpp"
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
  void EnableStencilBuffer() override;
  void DisableStencilBuffer() override;
  void EnableAlphaBlending() override;
  void DisableAlphaBlending() override;
  void BindFramebuffer(unsigned int frameBuffer) override;
  void UnbindFramebuffer() override;
  ZTexture LoadDefaultTexture() override;
  ZTexture LoadTexture(std::string path, const std::string &directory) override;
  void BindTexture(ZTexture texture, unsigned int index) override;
  ZTexture LoadDepthTexture() override;
  unsigned int LoadDepthMapBuffer(ZTexture depthTexture) override;
  void BindDepthMapBuffer(unsigned int frameBuffer) override;
  void UnbindDepthMapBuffer() override;
  unsigned int LoadCubeMap(std::vector<std::string> faces) override;
  ZBufferData LoadVertexData(std::vector<ZVertex> vertices, std::vector<unsigned int> indices) override;
  ZBufferData LoadVertexData(std::vector<float> vertices) override;
  void Draw(ZBufferData bufferData, std::vector<ZVertex> vertices, std::vector<unsigned int> indices) override;
  void Draw(ZBufferData bufferData, std::vector<float> vertices) override;
  void CleanUp() override;

protected:

};
