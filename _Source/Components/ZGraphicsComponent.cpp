//
//  ZGraphicsComponent.cpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#include "ZGraphicsComponent.hpp"
#include "ZEngine.hpp"
#include "ZDomain.hpp"
#include "ZGameObject.hpp"
#include "ZCameraComponent.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZOFTree.hpp"

ZGraphicsComponent::ZGraphicsComponent() : ZComponent() {
  highlightColor_ = glm::vec4(0.f);
}

void ZGraphicsComponent::Initialize(ZModel* model, ZShader* shader) {
  model_ = model;

  if (shader != nullptr) {
    shaders_.push_back(shader);
    ++activeShaderIndex_;
  }
}

void ZGraphicsComponent::Initialize(ZOFNode* root) {
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZGraphicsComponent", ZERROR);
    return;
  }

  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() > 0) {
      if (prop->id == "activeShader") {
        ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
        activeShaderIndex_ = terminal->value;
      } else if (prop->id == "highlightColor") {
        ZOFNumberList* terminal = dynamic_cast<ZOFNumberList*>(prop->values[0]);
        highlightColor_ = glm::vec4(terminal->value[0], terminal->value[1], terminal->value[2], 1.f);
      } else if (prop->id == "highlightShader") {
        ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
        // TODO: look through node->root->children for the given shader id and create it
      } else if (prop->id == "shaders") {
        ZOFStringList* terminal = dynamic_cast<ZOFStringList*>(prop->values[0]);
        // TODO: look through node->root->children for the given shader ids and create them
      } else if (prop->id == "model") {
        ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
        // TODO: If the first value is a string literal (i.e. surrounded by "") then load the model at that path
        // Otherwise create the given primitive. Set the scale to the second value.
      } else if (prop->id == "textures") {
        ZOFStringList* terminal = dynamic_cast<ZOFStringList*>(prop->values[0]);
        // TODO: look through node->root->children for the given texture id and create it
      }
    }
  }
}

void ZGraphicsComponent::Update(const std::map<std::string, ZLight*>& gameLights, ZGameObject* camera, float frameMix, unsigned char renderOp) {
  const ZDomain* domain = ZEngine::Domain();

  ZCameraComponent* cameraComp = camera->FindComponent<ZCameraComponent>();
  assert(cameraComp != nullptr);

  if (cameraComp->Type() == ZCameraType::Orthographic) {
    float left = -(float)domain->WindowWidth() / (cameraComp->GetZoom() * 2);
    float right = -left;
    float bottom = -(float)domain->WindowHeight() / (cameraComp->GetZoom() * 2);
    float top = -bottom;
    projectionMatrix_ = glm::ortho(left, right, bottom, top, -cameraComp->GetFarField() / 2.f, cameraComp->GetFarField());
  } else {
    projectionMatrix_ = glm::perspective(glm::radians(cameraComp->GetZoom()),
                                         (float)domain->WindowWidth() / (float)domain->WindowHeight(),
                                         cameraComp->GetNearField(), cameraComp->GetFarField());
  }

  viewMatrix_ = translatesWithView_ ? cameraComp->ViewMatrix(frameMix) : glm::mat4(glm::mat3(cameraComp->ViewMatrix(frameMix)));

  // Makes sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
  ZEngine::Graphics()->Strategy()->EnableStencilBuffer();

  ZShader* shader = (renderOp & ZGraphics::RENDER_OP_SHADOW) == ZGraphics::RENDER_OP_SHADOW ? ZEngine::Graphics()->ShadowShader() : GetActiveShader();

  shader->Activate();

  shader->Use(gameLights);

  ZEngine::Graphics()->Strategy()->BindTexture(ZEngine::Graphics()->DepthMap(), 0);

  shader->SetMat4("P", projectionMatrix_);
  shader->SetMat4("V", viewMatrix_);
  shader->SetMat4("M", object_->ModelMatrix());
  shader->SetMat4("P_lightSpace", ZEngine::Graphics()->LightSpaceMatrix());
  shader->SetVec3("viewDirection", camera->Front());

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

  glm::mat4 highlightModelMatrix_ = glm::scale(object_->ModelMatrix(), glm::vec3(1.07f, 1.07f, 1.07f));

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
