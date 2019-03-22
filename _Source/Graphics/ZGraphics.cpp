/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGLGraphics.cpp

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

// TODO: Conditional include based on graphics implementation
#include "ZGLGraphicsStrategy.hpp"
#include "ZGraphicsFactory.hpp"
#include "ZDomainStrategy.hpp"
#include "ZGameObject.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZShader.hpp"
#include "ZLight.hpp"

void ZGraphics::Initialize() {
  // TODO: Switch the strategies here based on implementation details
  if (graphicsStrategy_ == nullptr) {
    graphicsStrategy_ = new ZGLGraphicsStrategy();
    graphicsStrategy_->Initialize();
    depthMap_ = graphicsStrategy_->LoadDepthTexture();
    depthFramebuffer_ = graphicsStrategy_->LoadDepthMapBuffer(depthMap_);
  }
}

void ZGraphics::Load(ZOFTree* root) {
  ZShaderMap shaders = ZEngine::GraphicsFactory()->CreateShaders(root);
  for (ZShaderMap::iterator it = shaders.begin(); it != shaders.end(); it++) {
    AddShader(it->first, it->second);
  }

  ZTextureMap textures = ZEngine::GraphicsFactory()->CreateTextures(root);
  for (ZTextureMap::iterator it = textures.begin(); it != textures.end(); it++) {
    AddTexture(it->first, it->second);
  }
}

void ZGraphics::Draw(const ZGameObjectMap& gameObjects, const ZLightMap& gameLights, float frameMix) {
  if (!ZEngine::Domain()->Strategy()->IsWindowClosing()) {
    graphicsStrategy_->ClearViewport();

    // TODO: Support more shadow casting lights!
    if (gameLights.size() > 0) {
      DrawShadowMap(gameObjects, gameLights.begin()->second, frameMix);
    }

    Render(gameObjects, frameMix);
  }
}

void ZGraphics::Render(const ZGameObjectMap& gameObjects, float frameMix, unsigned char renderOp) {
  for (auto it = gameObjects.begin(); it != gameObjects.end(); it++) {
    it->second->Render(frameMix, renderOp);
  }
}

void ZGraphics::DrawShadowMap(const ZGameObjectMap& gameObjects, std::shared_ptr<ZLight> light, float frameMix) {
  graphicsStrategy_->BindDepthMapBuffer(depthFramebuffer_);

  if (shadowShader_ == nullptr) {
    shadowShader_ = std::shared_ptr<ZShader>(new ZShader);
    shadowShader_->Initialize("Assets/Shaders/Vertex/shadow.vert", "Assets/Shaders/Pixel/shadow.frag");
  }
  shadowShader_->Activate();

  // TODO: For now we support one light source for shadows, but this should change
  // so that multiple light space matrices are supported for multiple light sources
  // that can cast shadows, possibly using deferred rendering
  // TODO: Do something about these magic numbers!
  glm::mat4 lightP = glm::ortho(-25.f, 25.f, -25.f, 25.f, -100.f, 100.f);
  glm::mat4 lightV = glm::lookAt(light->type == ZLightType::Directional ?
                                  light->directional.direction :
                                  light->Position(), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
  glm::mat4 lightSpaceMatrix = lightP * lightV;
  currentLightSpaceMatrix_ = lightSpaceMatrix;

  Render(gameObjects, frameMix, RENDER_OP_SHADOW);

  graphicsStrategy_->UnbindDepthMapBuffer();
}

void ZGraphics::AddShader(std::string id, std::shared_ptr<ZShader> shader) {
  if (shader != nullptr) loadedShaders_[id] = shader;
}

void ZGraphics::AddTexture(std::string id, ZTexture texture) {
  loadedTextures_[id] = texture;
}

void ZGraphics::CleanUp() {
  if (graphicsStrategy_ != nullptr) {
    graphicsStrategy_ = nullptr;
  }

  if (shadowShader_ != nullptr) {
    shadowShader_ = nullptr;
  }
}
