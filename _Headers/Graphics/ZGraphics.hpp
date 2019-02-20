//
//  ZGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
class ZGameObject;
class ZWindow;
class ZShader;
class ZGraphicsStrategy;
struct ZLight;

// Class and Data Structure Definitions
typedef std::map<std::string, ZTexture> ZTextureMap;

class ZGraphics {
private:
  void Render(const ZGameObjectMap& gameObjects, float frameMix, unsigned char renderOp = ZGraphics::RENDER_OP_COLOR);
  void DrawShadowMap(const ZGameObjectMap& gameObjects, ZLight* light, float frameMix = 0.0);

public:
  static unsigned char RENDER_OP_COLOR;
  static unsigned char RENDER_OP_SHADOW;

  ZGraphics() : depthFramebuffer_(0) { }
  ~ZGraphics() { }

  void Initialize();

  void Draw(const ZGameObjectMap& gameObjects, const std::map<std::string, ZLight*>& gameLights, float frameMix = 0.0);

  ZGraphicsStrategy* Strategy() { return graphicsStrategy_; }
  glm::mat4 LightSpaceMatrix() { return currentLightSpaceMatrix_; }
  unsigned int DepthFrameBuffer() { return depthFramebuffer_; }
  ZTexture DepthMap() { return depthMap_; };
  ZShader* ShadowShader() { return shadowShader_; }
  ZTextureMap& Textures() { return loadedTextures_; }
  ZShaderMap& Shaders() { return loadedShaders_; }

  void SetStrategy(ZGraphicsStrategy* strategy) { graphicsStrategy_ = strategy; }

  void AddShader(std::string id, ZShader* shader);
  void AddTexture(std::string id, ZTexture texture);

  void CleanUp();

protected:
  ZShader* shadowShader_ = nullptr;
  ZGraphicsStrategy* graphicsStrategy_ = nullptr;
  unsigned int depthFramebuffer_;
  ZTexture depthMap_;
  glm::mat4 currentLightSpaceMatrix_;
  ZShaderMap loadedShaders_;
  ZTextureMap loadedTextures_;
};
