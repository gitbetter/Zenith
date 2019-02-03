//
//  ZGraphicsComponent.cpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#include "ZEngine.hpp"
#include "ZWindow.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZCamera.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

ZGraphicsComponent::ZGraphicsComponent(ZModel* model, ZShader* shader) : model_(model), modelMatrix_(1.f), highlightColor_(0) {
  if (shader != nullptr) {
    shaders_.push_back(shader);
    ++activeShaderIndex_;
  }
  modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(0.2f, 0.2f, 0.2f));
}

void ZGraphicsComponent::Update(ZCamera* camera, float frameMix) {
  // TODO: Use frameMix to interpolate between frames
  ZShader* shader = GetActiveShader();
  const ZWindow* window = ZEngine::GetGraphics()->GetWindow();

  if (camera->Type() == ZCameraType::Orthographic) {
    float left = -(float)window->GetWidth() / (camera->GetZoom() * 2);
    float right = -left;
    float bottom = -(float)window->GetHeight() / (camera->GetZoom() * 2);
    float top = -bottom;
    projectionMatrix_ = glm::ortho(left, right, bottom, top, -camera->GetFarField() / 2.f, camera->GetFarField());
  } else {
    projectionMatrix_ = glm::perspective(glm::radians(camera->GetZoom()),
                                         (float)window->GetWidth() / (float)window->GetHeight(),
                                         camera->GetNearField(), camera->GetFarField());
  }

  viewMatrix_ = translatesWithView_ ? camera->GetViewMatrix() : glm::mat4(glm::mat3(camera->GetViewMatrix()));

  // Make sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0xFF);

  // TODO: Set to ZGraphicsComponent transform property
  // model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
  shader->Activate();
  shader->SetMat4("P", projectionMatrix_);
  shader->SetMat4("V", viewMatrix_);
  shader->SetMat4("M", modelMatrix_);
  shader->SetVec3("viewDirection", camera->GetFrontVector());

  model_->Render(shader);

  DrawOutlineIfEnabled();
}

void ZGraphicsComponent::SetOutline(glm::vec4 color) {
  if (highlightShader_ == nullptr)
    highlightShader_ = new ZShader("Resources/Shaders/Vertex/basic.vert", "Resources/Shaders/Pixel/outline.frag");
  highlightColor_ = color;
}

void ZGraphicsComponent::DrawOutlineIfEnabled() {
  if (highlightShader_ == nullptr) return;

  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilMask(0x00);
  glDisable(GL_DEPTH_TEST);

  highlightShader_->Activate();

  glm::mat4 highlightModelMatrix_ = glm::scale(modelMatrix_, glm::vec3(1.07f));

  highlightShader_->SetMat4("P", projectionMatrix_);
  highlightShader_->SetMat4("V", viewMatrix_);
  highlightShader_->SetMat4("M", highlightModelMatrix_);
  highlightShader_->SetVec4("highlightColor", highlightColor_);

  model_->Render(highlightShader_);

  glStencilMask(0xFF);
  glEnable(GL_DEPTH_TEST);
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
