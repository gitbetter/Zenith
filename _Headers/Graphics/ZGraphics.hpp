//
//  ZGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphicsCommon.hpp"
#include <vector>
#include <glm/glm.hpp>

// Forward Declarations
class ZGameObject;
class ZWindow;
class ZShader;
class ZGraphicsStrategy;
struct ZLight;

// Class and Data Structure Definitions
class ZGraphics {
private:
  void Render(const std::vector<ZGameObject*>& gameObjects, float frameMix, unsigned char renderOp = ZGraphics::RENDER_OP_COLOR);
  void DrawShadowMap(const std::vector<ZGameObject*>& gameObjects, ZLight* light, float frameMix = 0.0);

public:
  static unsigned char RENDER_OP_COLOR;
  static unsigned char RENDER_OP_SHADOW;

  ZGraphics() : depthFramebuffer_(0) { }
  ~ZGraphics() { }

  void Initialize();

  void Draw(const std::vector<ZGameObject*>& gameObjects, const std::vector<ZLight*>& gameLights, float frameMix = 0.0);
  void CleanUp();

  glm::mat4 LightSpaceMatrix() { return currentLightSpaceMatrix_; }
  unsigned int DepthFrameBuffer() { return depthFramebuffer_; }
  ZTexture DepthMap() { return depthMap_; };
  ZShader* ShadowShader() { return shadowShader_; }

  void SetStrategy(ZGraphicsStrategy* strategy) { graphicsStrategy_ = strategy; }
  ZGraphicsStrategy* Strategy() { return graphicsStrategy_; }

protected:
  ZShader* shadowShader_ = nullptr;
  ZGraphicsStrategy* graphicsStrategy_ = nullptr;
  unsigned int depthFramebuffer_;
  ZTexture depthMap_;
  glm::mat4 currentLightSpaceMatrix_;
};
