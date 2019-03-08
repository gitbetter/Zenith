//
//  ZGraphicsComponent.cpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#include "ZGraphicsComponent.hpp"
#include "ZGraphicsFactory.hpp"
#include "ZEngine.hpp"
#include "ZDomain.hpp"
#include "ZCameraComponent.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZGame.hpp"
#include "ZSkybox.hpp"
#include "ZOFTree.hpp"

ZGraphicsComponent::ZGraphicsComponent() : ZComponent() {
  highlightColor_ = glm::vec4(0.f);
  id_ = "ZCGraphics_" + ZEngine::IDSequence()->Next();
}

void ZGraphicsComponent::Initialize(std::shared_ptr<ZModel> model, std::shared_ptr<ZShader> shader) {
  model_ = model;

  if (shader != nullptr) {
    shaders_.push_back(shader);
    ++activeShaderIndex_;
  }
}

// TODO: These initalize functions can get pretty hectic. Maybe there's a better way...
void ZGraphicsComponent::Initialize(ZOFNode* root) {
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZGraphicsComponent", ZERROR);
    return;
  }

  unsigned int activeShaderIndex = -1;
  glm::vec4 highlightColor(0.f);
  std::shared_ptr<ZShader> highlightShader;
  std::vector<std::shared_ptr<ZShader>> shaders;
  std::shared_ptr<ZModel> model;
  std::vector<std::shared_ptr<ZMaterial>> materials;

  for (ZOFPropertyNode* prop : node->properties) {
    if (prop->values.size() == 0) continue;

    if (prop->id == "activeShader") {
      ZOFNumber* terminal = dynamic_cast<ZOFNumber*>(prop->values[0]);
      activeShaderIndex = terminal->value;
    } else if (prop->id == "highlightColor") {
      ZOFNumberList* terminal = dynamic_cast<ZOFNumberList*>(prop->values[0]);
      highlightColor = glm::vec4(terminal->value[0], terminal->value[1], terminal->value[2], 1.f);
    } else if (prop->id == "highlightShader") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      if (ZEngine::Graphics()->Shaders().find(terminal->value) != ZEngine::Graphics()->Shaders().end()) {
        highlightShader = ZEngine::Graphics()->Shaders()[terminal->value];
      }
    } else if (prop->id == "shaders") {
      ZOFStringList* terminal = dynamic_cast<ZOFStringList*>(prop->values[0]);
      for (std::string id : terminal->value) {
        if (ZEngine::Graphics()->Shaders().find(id) != ZEngine::Graphics()->Shaders().end()) {
          shaders.push_back(ZEngine::Graphics()->Shaders()[id]);
        }
      }
    } else if (prop->id == "model") {
      ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
      if (terminal->value.find("\"") == 0) {
        model = std::shared_ptr<ZModel>(new ZModel(terminal->value.substr(1, terminal->value.length() - 2)));
      } else {
        if (prop->values.size() > 1) {
          ZOFNumberList* param = dynamic_cast<ZOFNumberList*>(prop->values[1]);
          model = ZEngine::GraphicsFactory()->CreateModel(terminal->value, glm::vec3(param->value[0], param->value[1], param->value[2]));
        } else {
          model = ZEngine::GraphicsFactory()->CreateModel(terminal->value);
        }
      }
    }
  }

  // TODO: If there are any material subcomponents for this graphics component, we parse them with this loop
  for (ZOFChildMap::iterator matIt = node->children.begin(); matIt != node->children.end(); matIt++) {
    if (matIt->first == "Material") {
      std::shared_ptr<ZMaterial> material = std::make_shared<ZMaterial>();
      material->Initialize(matIt->second);
      materials.push_back(material);
    }
  }
  if (materials.empty()) materials.push_back(ZMaterial::DefaultMaterialPBR());

  activeShaderIndex_ = activeShaderIndex;
  highlightColor_ = highlightColor;
  highlightShader_ = highlightShader;
  shaders_ = shaders;
  model_ = model;
  materials_ = materials;
}

void ZGraphicsComponent::Render(float frameMix, RENDER_OP renderOp) {
  if (gameCamera_ == nullptr) return;

  std::shared_ptr<ZCameraComponent> cameraComp = gameCamera_->FindComponent<ZCameraComponent>();

  glm::mat4 modelMatrix = object_->ModelMatrix();
  glm::mat4 projectionMatrix = cameraComp->ProjectionMatrix();
  glm::mat4 viewMatrix = cameraComp->ViewMatrix(frameMix);

  // Makes sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
  ZEngine::Graphics()->Strategy()->EnableStencilBuffer();

  std::shared_ptr<ZShader> shader = (renderOp & RENDER_OP_SHADOW) == RENDER_OP_SHADOW ? ZEngine::Graphics()->ShadowShader() : ActiveShader();
  if (renderOp & (RENDER_OP_COLOR == RENDER_OP_COLOR)) shader->SetInt("shadowMap", 0);

  shader->Activate();
  shader->Use(gameLights_);

  ZEngine::Graphics()->Strategy()->BindTexture(ZEngine::Graphics()->DepthMap(), 0);

  shader->SetMat4("P", projectionMatrix);
  shader->SetMat4("V", viewMatrix);
  shader->SetMat4("M", modelMatrix);
  shader->SetMat4("P_lightSpace", ZEngine::Graphics()->LightSpaceMatrix());
  shader->SetVec3("viewDirection", gameCamera_->Front());

  if (object_->Game()->Skybox() != nullptr) {
    ZEngine::Graphics()->Strategy()->BindTexture(object_->Game()->Skybox()->IBLTexture().irradiance, 1);
    shader->SetInt("irradianceMap", 1);
    ZEngine::Graphics()->Strategy()->BindTexture(object_->Game()->Skybox()->IBLTexture().prefiltered, 2);
    shader->SetInt("prefilterMap", 2);
    ZEngine::Graphics()->Strategy()->BindTexture(object_->Game()->Skybox()->IBLTexture().brdfLUT, 3);
    shader->SetInt("brdfLUT", 3);
  }

  model_->Render(shader.get(), materials_);

  DrawOutlineIfEnabled(modelMatrix, viewMatrix, projectionMatrix);
}

void ZGraphicsComponent::SetOutline(glm::vec4 color) {
  if (highlightShader_ == nullptr) {
    highlightShader_ = std::shared_ptr<ZShader>(new ZShader);
    highlightShader_->Initialize("Assets/Shaders/Vertex/blinnphong.vert", "Assets/Shaders/Pixel/outline.frag");
  }
  highlightColor_ = color;
}

void ZGraphicsComponent::AddMaterial(std::shared_ptr<ZMaterial> material) {
  materials_.push_back(material);
}

void ZGraphicsComponent::DrawOutlineIfEnabled(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
  if (highlightShader_ == nullptr) return;

  ZEngine::Graphics()->Strategy()->DisableStencilBuffer();

  highlightShader_->Activate();

  glm::mat4 highlightModelMatrix = glm::scale(model, glm::vec3(1.07f, 1.07f, 1.07f));

  highlightShader_->SetMat4("P", projection);
  highlightShader_->SetMat4("V", view);
  highlightShader_->SetMat4("M", highlightModelMatrix);
  highlightShader_->SetVec4("highlightColor", highlightColor_);

  model_->Render(highlightShader_.get(), materials_);

  ZEngine::Graphics()->Strategy()->EnableStencilBuffer();
}

void ZGraphicsComponent::ClearOutline() {
  if (highlightShader_ != nullptr) highlightShader_ = nullptr;
}
