//
//  ZGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
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

  void Render(const ZGameObjectMap& gameObjects, float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR);
  void DrawShadowMap(const ZGameObjectMap& gameObjects, std::shared_ptr<ZLight> light, float frameMix = 0.0);

public:

  ZGraphics() { }
  ~ZGraphics() { }

  void Initialize();

  void Draw(const ZGameObjectMap& gameObjects, const std::map<std::string, std::shared_ptr<ZLight>>& gameLights, float frameMix = 0.0);

  ZGraphicsStrategy* Strategy() { return graphicsStrategy_; }
  glm::mat4 LightSpaceMatrix() { return currentLightSpaceMatrix_; }
  ZTexture DepthMap() { return depthMap_; };
  std::shared_ptr<ZShader> ShadowShader() { return shadowShader_; }
  ZTextureMap& Textures() { return loadedTextures_; }
  ZShaderMap& Shaders() { return loadedShaders_; }

  void SetStrategy(ZGraphicsStrategy* strategy) { graphicsStrategy_ = strategy; }

  void AddShader(std::string id, std::shared_ptr<ZShader> shader);
  void AddTexture(std::string id, ZTexture texture);

  void CleanUp();

protected:

  std::shared_ptr<ZShader> shadowShader_ = nullptr;
  ZGraphicsStrategy* graphicsStrategy_ = nullptr;
  ZBufferData depthFramebuffer_;
  ZTexture depthMap_;
  glm::mat4 currentLightSpaceMatrix_;
  ZShaderMap loadedShaders_;
  ZTextureMap loadedTextures_;
};
