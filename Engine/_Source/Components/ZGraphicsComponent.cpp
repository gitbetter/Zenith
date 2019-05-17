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
#include "ZScene.hpp"
#include "ZSkybox.hpp"
#include "ZOFTree.hpp"

ZGraphicsComponent::ZGraphicsComponent() : ZComponent() {
	highlightColor_ = glm::vec4(0.f);
	id_ = "ZCGraphics_" + zenith::IDSequence()->Next();
}

ZGraphicsComponent::~ZGraphicsComponent() {
	shaders_.clear();
	materials_.clear();
}

void ZGraphicsComponent::Initialize(std::shared_ptr<ZModel> model, std::shared_ptr<ZShader> shader) {
	modelObject_ = model;
	if (shader != nullptr) currentShaderObject_ = shader;
}

void ZGraphicsComponent::Initialize(std::shared_ptr<ZOFNode> root) {
	ZComponent::Initialize();

	std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
	if (node == nullptr) {
		_Z("Could not initalize ZGraphicsComponent", ZERROR);
		return;
	}

	ZOFPropertyMap props = node->properties;

	if (props.find("activeShader") != props.end() && props["activeShader"]->HasValues()) {
		std::shared_ptr<ZOFNumber> activeShaderProp = props["activeShader"]->Value<ZOFNumber>(0);
		activeShaderIndex_ = activeShaderProp->value;
	}

	if (props.find("highlightColor") != props.end() && props["highlightColor"]->HasValues()) {
		std::shared_ptr<ZOFNumberList> hColorProp = props["highlightColor"]->Value<ZOFNumberList>(0);
		highlightColor_ = glm::vec4(hColorProp->value[0], hColorProp->value[1], hColorProp->value[2], 1.f);
	}

	if (props.find("highlightShader") != props.end() && props["highlightShader"]->HasValues()) {
		std::shared_ptr<ZOFString> hShaderProp = props["highlightShader"]->Value<ZOFString>(0);
		if (zenith::Graphics()->Shaders().find(hShaderProp->value) != zenith::Graphics()->Shaders().end()) {
			highlightShader_ = zenith::Graphics()->Shaders()[hShaderProp->value];
		}
	}

	if (props.find("shaders") != props.end() && props["shaders"]->HasValues()) {
		std::shared_ptr<ZOFStringList> shadersProp = props["shaders"]->Value<ZOFStringList>(0);
		for (std::string id : shadersProp->value) {
            shaders_.push_back(id);
		}
	}

	if (props.find("model") != props.end() && props["model"]->HasValues()) {
		std::shared_ptr<ZOFString> nameProp = props["model"]->Value<ZOFString>(0);
		if (nameProp->value.find("ZMOD") != std::string::npos) {
            model_ = nameProp->value;
		} else {
			if (props["model"]->ValueCount() > 1) {
				std::shared_ptr<ZOFNumberList> scaleProp = props["model"]->Value<ZOFNumberList>(1);
				modelObject_ = zenith::GraphicsFactory()->CreateModel(nameProp->value, glm::vec3(scaleProp->value[0], scaleProp->value[1], scaleProp->value[2]));
			} else {
				modelObject_ = zenith::GraphicsFactory()->CreateModel(nameProp->value);
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

void ZGraphicsComponent::Render(ZRenderOp renderOp) {
	if (gameCamera_ == nullptr || modelObject_ == nullptr) return;

	std::shared_ptr<ZCameraComponent> cameraComp = gameCamera_->FindComponent<ZCameraComponent>();

	glm::mat4 modelMatrix = object_->Scene()->TopMatrix();
	glm::mat4 projectionMatrix = cameraComp->ProjectionMatrix();
	glm::mat4 viewMatrix = cameraComp->ViewMatrix();

	// Makes sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
	zenith::Graphics()->Strategy()->EnableStencilBuffer();

    std::shared_ptr<ZShader> shader;
    if (renderOp == ZRenderOp::Shadow) {
        shader = zenith::Graphics()->ShadowShader();
    } else if (renderOp == ZRenderOp::Depth) {
        shader = zenith::Graphics()->DepthShader();
    } else {
        shader = ActiveShader();
    }
    
    if (shader) {
        shader->Activate();
        shader->Use(gameLights_);

        zenith::Graphics()->Strategy()->BindTexture(zenith::Graphics()->DepthBuffer(), 0);
        zenith::Graphics()->Strategy()->BindTexture(zenith::Graphics()->ShadowBuffer(), 1);
        if (renderOp == ZRenderOp::Color) {
            shader->SetInt("depthTexture", 0);
            shader->SetInt("shadowTexture", 1);
        }

		shader->SetMat4("P", projectionMatrix);
		shader->SetMat4("V", viewMatrix);
        shader->SetMat4("M", modelMatrix);
		shader->SetMat4("ViewProjection", object_->Scene()->ViewProjection());
        shader->SetMat4("P_lightSpace", zenith::Graphics()->LightSpaceMatrix());
        shader->SetVec3("viewPosition", gameCamera_->Position());

        if (object_->Scene()->Skybox() != nullptr) {
            zenith::Graphics()->Strategy()->BindTexture(object_->Scene()->Skybox()->IBLTexture().irradiance, 3);
            shader->SetInt("irradianceMap", 3);
            zenith::Graphics()->Strategy()->BindTexture(object_->Scene()->Skybox()->IBLTexture().prefiltered, 4);
            shader->SetInt("prefilterMap", 4);
            zenith::Graphics()->Strategy()->BindTexture(object_->Scene()->Skybox()->IBLTexture().brdfLUT, 5);
            shader->SetInt("brdfLUT", 5);
        }

        modelObject_->Render(shader.get(), materials_);
    }

	DrawOutlineIfEnabled(modelMatrix, object_->Scene()->ViewProjection());
}

std::shared_ptr<ZShader> ZGraphicsComponent::ActiveShader() {
    if (currentShaderObject_) return currentShaderObject_;
    if (shaders_.empty()) return nullptr;
    
    if (zenith::Graphics()->Shaders().find(shaders_[activeShaderIndex_]) != zenith::Graphics()->Shaders().end()) {
		currentShaderObject_ = zenith::Graphics()->Shaders()[shaders_[activeShaderIndex_]];
        return currentShaderObject_;
    }
    return nullptr;
}

std::shared_ptr<ZModel> ZGraphicsComponent::Model() {
	if (modelObject_) return modelObject_;

    if (zenith::Graphics()->Models().find(model_) != zenith::Graphics()->Models().end()) {
        modelObject_ = zenith::Graphics()->Models()[model_];
        return modelObject_;
    }
	return nullptr;
}

void ZGraphicsComponent::SetOutline(glm::vec4 color) {
	if (highlightShader_ == nullptr) {
		highlightShader_ = std::shared_ptr<ZShader>(new ZShader("Assets/Shaders/Vertex/blinnphong.vert", "Assets/Shaders/Pixel/outline.frag"));
		highlightShader_->Initialize();
	}
	highlightColor_ = color;
}

void ZGraphicsComponent::AddMaterial(std::shared_ptr<ZMaterial> material) {
	materials_.push_back(material);
}

void ZGraphicsComponent::DrawOutlineIfEnabled(glm::mat4& model, glm::mat4& viewProjection) {
	if (highlightShader_ == nullptr) return;

	zenith::Graphics()->Strategy()->DisableStencilBuffer();

	highlightShader_->Activate();

	glm::mat4 highlightModelMatrix = glm::scale(model, glm::vec3(1.07f, 1.07f, 1.07f));

	highlightShader_->SetMat4("ViewProjection", viewProjection);
	highlightShader_->SetMat4("M", highlightModelMatrix);
	highlightShader_->SetVec4("color", highlightColor_);

	modelObject_->Render(highlightShader_.get(), materials_);

	zenith::Graphics()->Strategy()->EnableStencilBuffer();
}

void ZGraphicsComponent::ClearOutline() {
	if (highlightShader_ != nullptr) highlightShader_ = nullptr;
}