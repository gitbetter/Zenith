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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

ZGraphicsComponent::ZGraphicsComponent(ZModel* model, ZShader* shader) : model_(model) {
  if (shader != nullptr) {
    shaders_.push_back(shader);
    ++activeShaderIndex_;
  }
}

void ZGraphicsComponent::Update(ZCamera* camera, float frameMix) {
  // TODO: Use frameMix to interpolate between frames
  ZShader* shader = shaders_[activeShaderIndex_];
  const ZWindow* window = ZEngine::GetGraphics()->GetWindow();

  glm::mat4 projection = glm::perspective(glm::radians(camera->GetZoom()), (float)window->GetWidth() / (float)window->GetHeight(),
                                                       camera->GetNearField(), camera->GetFarField());
  glm::mat4 view = camera->GetViewMatrix();
  glm::mat4 model(1.0f);

  // TODO: Set to ZGraphicsComponent transform property
  // model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
  model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

  shader->SetMat4("P", projection);
  shader->SetMat4("V", view);
  shader->SetMat4("M", model);

  model_->Render(shader);
}
