/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGraphics.hpp

    Created by Adrian Sanchez on 27/01/2019.
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
  void Load(ZOFTree* root);

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
