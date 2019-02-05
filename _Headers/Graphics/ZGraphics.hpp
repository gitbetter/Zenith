//
//  ZGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZShader.hpp"
#include <vector>

// Forward Declarations
class ZGameObject;
class ZWindow;

// Class and Data Structure Definitions
class ZGraphics {
private:
public:
  ZGraphics() : depthFramebuffer_(0), depthMap_(0) { }
  virtual ~ZGraphics() { }

  virtual void Draw(const std::vector<ZGameObject*>& gameObjects, const std::vector<ZLight*>& gameLights, float frameMix = 0.0) = 0;
  virtual void SwapBuffers() = 0;
  virtual void EnableStencilBuffer() = 0;
  virtual void DisableStencilBuffer() = 0;
  virtual void GenerateDepthMap() = 0;
  virtual void DrawDepthMap(ZLight* light) = 0;
  virtual void UpdateWindowSize() { }
  virtual void Delete() = 0;
  virtual ZWindow* GetWindow() = 0;

protected:
  ZWindow* window_ = nullptr;
  ZShader* shadowShader_ = nullptr;
  unsigned int depthFramebuffer_, depthMap_;
};
