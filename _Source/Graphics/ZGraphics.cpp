//
//  ZGLGraphics.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

// TODO: Conditional include based on graphics implementation
#include "ZGLGraphicsStrategy.hpp"
#include "ZDomainStrategy.hpp"
#include "ZGameObject.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZShader.hpp"
#include "ZLight.hpp"

unsigned char ZGraphics::RENDER_OP_COLOR = 0x01;
unsigned char ZGraphics::RENDER_OP_SHADOW = 0x02;

void ZGraphics::Initialize() {
  // TODO: Switch the strategies here based on implementation details
  if (graphicsStrategy_ == nullptr) {
    graphicsStrategy_ = new ZGLGraphicsStrategy();
    graphicsStrategy_->Initialize();
    depthMap_ = graphicsStrategy_->LoadDepthTexture();
    depthFramebuffer_ = graphicsStrategy_->LoadDepthMapBuffer(depthMap_);
  }
}

void ZGraphics::Draw(const ZGameObjectMap& gameObjects, const std::map<std::string, ZLight*>& gameLights, float frameMix) {
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

void ZGraphics::DrawShadowMap(const ZGameObjectMap& gameObjects, ZLight* light, float frameMix) {
  graphicsStrategy_->BindDepthMapBuffer(depthFramebuffer_);

  if (shadowShader_ == nullptr) {
    shadowShader_ = new ZShader;
    shadowShader_->Initialize("Assets/Shaders/Vertex/shadow.vert", "Assets/Shaders/Pixel/shadow.frag");
    shadowShader_->SetInt("shadowMap", 0);
  }
  shadowShader_->Activate();

  // TODO: For now we support one light source for shadows, but this should change
  // so that multiple light space matrices are supported for multiple light sources
  // that can cast shadows, possibly using deferred rendering
  // TODO: Do something about these magic numbers!
  glm::mat4 lightP = glm::ortho(-25.f, 25.f, -25.f, 25.f, 1.f, 100.f);
  glm::mat4 lightV = glm::lookAt(light->Position(), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
  glm::mat4 lightSpaceMatrix = lightP * lightV;
  currentLightSpaceMatrix_ = lightSpaceMatrix;

  Render(gameObjects, frameMix, ZGraphics::RENDER_OP_SHADOW);

  graphicsStrategy_->UnbindDepthMapBuffer();
}

void ZGraphics::AddShader(std::string id, ZShader* shader) {
  if (shader != nullptr) loadedShaders_[id] = shader;
}

void ZGraphics::AddTexture(std::string id, ZTexture texture) {
  loadedTextures_[id] = texture;
}

void ZGraphics::CleanUp() {
  if (graphicsStrategy_ != nullptr) {
    delete graphicsStrategy_;
  }

  if (shadowShader_ != nullptr) {
    delete shadowShader_;
  }
}
