//
//  ZGraphicsComponent.cpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#include "ZGraphicsComponent.hpp"
#include "ZEngine.hpp"
#include "ZDomain.hpp"
#include "ZCamera.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZCommon.hpp"

ZGraphicsComponent::ZGraphicsComponent(ZModel* model, ZShader* shader) : model_(model), modelMatrix_(1.f), highlightColor_(0) {
  if (shader != nullptr) {
    shaders_.push_back(shader);
    ++activeShaderIndex_;
  }
  modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(0.2f, 0.2f, 0.2f));
}

void ZGraphicsComponent::Update(const std::vector<ZLight*>& gameLights, ZCamera* camera, float frameMix, unsigned char renderOp) {
  // TODO: Use frameMix to interpolate between frames
  const ZDomain* domain = ZEngine::Domain();

  if (camera->Type() == ZCameraType::Orthographic) {
    float left = -(float)domain->WindowWidth() / (camera->GetZoom() * 2);
    float right = -left;
    float bottom = -(float)domain->WindowHeight() / (camera->GetZoom() * 2);
    float top = -bottom;
    projectionMatrix_ = glm::ortho(left, right, bottom, top, -camera->GetFarField() / 2.f, camera->GetFarField());
  } else {
    projectionMatrix_ = glm::perspective(glm::radians(camera->GetZoom()),
                                         (float)domain->WindowWidth() / (float)domain->WindowHeight(),
                                         camera->GetNearField(), camera->GetFarField());
  }

  viewMatrix_ = translatesWithView_ ? camera->ViewMatrix() : glm::mat4(glm::mat3(camera->ViewMatrix()));

  // Makes sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
  ZEngine::Graphics()->Strategy()->EnableStencilBuffer();

  ZShader* shader = (renderOp & ZGraphics::RENDER_OP_SHADOW) == ZGraphics::RENDER_OP_SHADOW ? ZEngine::Graphics()->ShadowShader() : GetActiveShader();

  shader->Activate();

  shader->Use(gameLights);

  ZEngine::Graphics()->Strategy()->BindTexture(ZEngine::Graphics()->DepthMap(), 0);

  shader->SetMat4("M", modelMatrix_);
  shader->SetMat4("V", viewMatrix_);
  shader->SetMat4("P", projectionMatrix_);
  shader->SetMat4("P_lightSpace", ZEngine::Graphics()->LightSpaceMatrix());
  shader->SetVec3("viewDirection", camera->GetFrontVector());

  model_->Render(shader);

  DrawOutlineIfEnabled();
}

void ZGraphicsComponent::SetOutline(glm::vec4 color) {
  if (highlightShader_ == nullptr)
    highlightShader_ = new ZShader("Assets/Shaders/Vertex/basic.vert", "Assets/Shaders/Pixel/outline.frag");
  highlightColor_ = color;
}

void ZGraphicsComponent::DrawOutlineIfEnabled() {
  if (highlightShader_ == nullptr) return;

  ZEngine::Graphics()->Strategy()->DisableStencilBuffer();

  highlightShader_->Activate();

  glm::mat4 highlightModelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.07f, 1.07f, 1.07f));

  highlightShader_->SetMat4("P", projectionMatrix_);
  highlightShader_->SetMat4("V", viewMatrix_);
  highlightShader_->SetMat4("M", highlightModelMatrix_);
  highlightShader_->SetVec4("highlightColor", highlightColor_);

  model_->Render(highlightShader_);

  ZEngine::Graphics()->Strategy()->EnableStencilBuffer();
}

void ZGraphicsComponent::ClearOutline() {
  if (highlightShader_ != nullptr) delete highlightShader_;
}

void ZGraphicsComponent::Scale(glm::vec3 scale) {
  modelMatrix_ = glm::scale(modelMatrix_, scale);
}

void ZGraphicsComponent::Translate(glm::vec3 translate) {
  modelMatrix_ = glm::translate(modelMatrix_, translate);
}

void ZGraphicsComponent::Rotate(float angle, glm::vec3 rotationAxis) {
  modelMatrix_ = glm::rotate(modelMatrix_, angle, rotationAxis);
}
