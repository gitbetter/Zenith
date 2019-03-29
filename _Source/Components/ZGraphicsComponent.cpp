/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
  ZGraphicsComponent.cpp

  Created by Adrian Sanchez on 26/01/2019.
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

ZGraphicsComponent::~ZGraphicsComponent() {
  shaders_.clear();
  materials_.clear();
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
  ZComponent::Initialize();
  
  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZGraphicsComponent", ZERROR);
    return;
  }

  ZOFPropertyMap props = node->properties;

  if (props.find("activeShader") != props.end() && props["activeShader"]->HasValues()) {
    ZOFNumber* activeShaderProp = props["activeShader"]->Value<ZOFNumber>(0);
    activeShaderIndex_ = activeShaderProp->value;
  }

  if (props.find("highlightColor") != props.end() && props["highlightColor"]->HasValues()) {
    ZOFNumberList* hColorProp = props["highlightColor"]->Value<ZOFNumberList>(0);
    highlightColor_ = glm::vec4(hColorProp->value[0], hColorProp->value[1], hColorProp->value[2], 1.f);
  }

  if (props.find("highlightShader") != props.end() && props["highlightShader"]->HasValues()) {
    ZOFString* hShaderProp = props["highlightShader"]->Value<ZOFString>(0);
    if (ZEngine::Graphics()->Shaders().find(hShaderProp->value) != ZEngine::Graphics()->Shaders().end()) {
      highlightShader_ = ZEngine::Graphics()->Shaders()[hShaderProp->value];
    }
  }

  if (props.find("shaders") != props.end() && props["shaders"]->HasValues()) {
    ZOFStringList* shadersProp = props["shaders"]->Value<ZOFStringList>(0);
    for (std::string id : shadersProp->value) {
      if (ZEngine::Graphics()->Shaders().find(id) != ZEngine::Graphics()->Shaders().end()) {
        shaders_.push_back(ZEngine::Graphics()->Shaders()[id]);
      }
    }
  }

  if (props.find("model") != props.end() && props["model"]->HasValues()) {
    ZOFString* nameProp = props["model"]->Value<ZOFString>(0);
    if (nameProp->value.find(".") != std::string::npos) {
      model_ = std::shared_ptr<ZModel>(new ZModel(nameProp->value));
    } else {
      if (props["model"]->ValueCount() > 1) {
        ZOFNumberList* scaleProp = props["model"]->Value<ZOFNumberList>(1);
        model_ = ZEngine::GraphicsFactory()->CreateModel(nameProp->value, glm::vec3(scaleProp->value[0], scaleProp->value[1], scaleProp->value[2]));
      } else {
        model_ = ZEngine::GraphicsFactory()->CreateModel(nameProp->value);
      }
    }
  }

  // If there are any material subcomponents for this graphics component, we parse them with this loop
  for (ZOFChildMap::iterator matIt = node->children.begin(); matIt != node->children.end(); matIt++) {
    if (matIt->first == "Material") {
      std::shared_ptr<ZMaterial> material = std::make_shared<ZMaterial>();
      material->Initialize(matIt->second);
      materials_.push_back(material);
    }
  }
  if (materials_.empty()) materials_.push_back(ZMaterial::DefaultMaterialPBR());
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
  shader->SetVec3("viewPosition", gameCamera_->Position());

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
  highlightShader_->SetVec4("color", highlightColor_);

  model_->Render(highlightShader_.get(), materials_);

  ZEngine::Graphics()->Strategy()->EnableStencilBuffer();
}

void ZGraphicsComponent::ClearOutline() {
  if (highlightShader_ != nullptr) highlightShader_ = nullptr;
}
