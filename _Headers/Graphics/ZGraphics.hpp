//
//  ZGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>
#include <glm/glm.hpp>

// Forward Declarations
class ZGameObject;
class ZWindow;
class ZShader;
struct ZLight;

// Class and Data Structure Definitions
class ZGraphics {
private:
public:
  ZGraphics() : depthFramebuffer_(0), depthMap_(0) { }
  virtual ~ZGraphics() { }

  static unsigned char RENDER_OP_COLOR;
  static unsigned char RENDER_OP_DEPTH;

  virtual void Draw(const std::vector<ZGameObject*>& gameObjects, const std::vector<ZLight*>& gameLights, float frameMix = 0.0) = 0;
  virtual void SwapBuffers() = 0;
  virtual void EnableStencilBuffer() = 0;
  virtual void DisableStencilBuffer() = 0;
  virtual void GenerateDepthMap() = 0;
  virtual void DrawDepthMap(const std::vector<ZGameObject*>& gameObjects, ZLight* light, float frameMix) = 0;
  virtual void BindDepthMap() = 0;
  virtual void UpdateWindowSize() { }
  virtual void Delete() = 0;
  virtual ZWindow* GetWindow() = 0;

  glm::mat4 LightSpaceMatrix() { return currentLightSpaceMatrix_; }
  unsigned int DepthMap() { return depthMap_; };
  ZShader* ShadowShader() { return shadowShader_; }

protected:
  ZWindow* window_ = nullptr;
  ZShader* shadowShader_ = nullptr;
  unsigned int depthFramebuffer_, depthMap_;
  glm::mat4 currentLightSpaceMatrix_;
};
