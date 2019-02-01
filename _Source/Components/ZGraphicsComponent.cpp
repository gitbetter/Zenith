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

  projectionMatrix_ = glm::perspective(glm::radians(camera->GetZoom()), (
                                                float)window->GetWidth() / (float)window->GetHeight(),
                                                camera->GetNearField(), camera->GetFarField());
  viewMatrix_ = camera->GetViewMatrix();

  // Make sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0xFF);

  // TODO: Set to ZGraphicsComponent transform property
  // model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
  shader->Activate();
  shader->SetMat4("P", projectionMatrix_);
  shader->SetMat4("V", viewMatrix_);
  shader->SetMat4("M", modelMatrix_);
  shader->SetVec3("viewDirection", glm::vec3(camera->GetFrontVector()));

  // TODO: Extract
  int materialIndex = 0;
  shader->SetVec4("materials[" + std::to_string(materialIndex) + "].albedo", glm::vec4(0.64f, 0.63f, 0.51f, 1.f));
  shader->SetVec3("materials[" + std::to_string(materialIndex) + "].diffuse", glm::vec3(1.0f));
  shader->SetVec3("materials[" + std::to_string(materialIndex) + "].ambient", glm::vec3(0.5f));
  shader->SetVec3("materials[" + std::to_string(materialIndex) + "].specular", glm::vec3(0.3f));
  shader->SetFloat("materials[" + std::to_string(materialIndex) + "].shininess", 20.0f);

  // TODO: Extract
  shader->SetBool("lights[0].isDirectional", true);
  shader->SetBool("lights[0].isEnabled", true);
  shader->SetVec3("lights[0].ambient", glm::vec3(0.3f));
  shader->SetVec3("lights[0].color", glm::vec3(0.8f));
  shader->SetVec3("lights[0].direction", glm::vec3(0.3f, 0.9f, 0.3f));

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
