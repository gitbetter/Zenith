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
  void EnableSeamlessCubemap() override;
  void DisableSeamlessCubemap() override;
  void EnableMSAA() override;
  void DisableMSAA() override;
	void CullFrontFaces() override;
	void CullBackFaces() override;
  void BindFramebuffer(ZBufferData frameBuffer) override;
  void UnbindFramebuffer() override;
  ZTexture LoadDefaultTexture() override;
  ZTexture LoadTexture(std::string path, const std::string &directory, bool hdr = false, bool flip = false) override;
  ZTexture LoadEmptyLUT() override;
  void BindTexture(ZTexture texture, unsigned int index) override;
  ZTexture LoadDepthTexture() override;
  ZBufferData LoadDepthMapBuffer(ZTexture depthTexture) override;
  ZBufferData LoadCubeMapBuffer() override;
  void BindDepthMapBuffer(ZBufferData frameBuffer) override;
  void UnbindDepthMapBuffer() override;
  void BindCubeMapBuffer(ZBufferData cubemapBuffer) override;
  ZTexture LoadCubeMap(std::vector<std::string> faces) override;
  ZTexture LoadEmptyCubeMap(ZCubemapTextureType type = ZCubemapTextureType::Normal) override;
  ZBufferData LoadIndexedVertexData(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) override;
  ZBufferData LoadVertexData(std::vector<ZVertex3D> vertices) override;
  ZBufferData LoadVertexData(std::vector<ZVertex2D> vertices) override;
  ZBufferData LoadEmptyVertexData2D(unsigned int size) override;
  void DeleteBufferData(ZBufferData bufferData) override;
  void UpdateBuffer(ZBufferData buffer, std::vector<ZVertex2D> data) override;

  ZTexture EquirectToCubemap(std::string equirectHDRPath, ZBufferData& bufferData) override;
  ZTexture IrradianceMapFromCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) override;
  ZTexture PrefilterCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) override;
  ZTexture BRDFLUT(ZBufferData cubemapBufferData) override;

  void Draw(ZBufferData bufferData, std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle) override;
  void Draw(ZBufferData bufferData, std::vector<ZVertex2D> vertices, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::TriangleStrip) override;
  void DrawLines(ZBufferData bufferData, std::vector<ZVertex3D> vertices) override;

protected:

  std::map<ZMeshDrawStyle, unsigned int> drawingStylesMap_;

};
