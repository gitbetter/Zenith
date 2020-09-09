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
#include "ZDomain.hpp"
#include "ZCameraComponent.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZScene.hpp"
#include "ZSkybox.hpp"
#include "ZOFTree.hpp"

ZGraphicsComponent::ZGraphicsComponent() : ZComponent(), highlightColor_(0.f), isBillboard_(false)
{
    id_ = "ZCGraphics_" + zenith::IDSequence()->Next();
}

ZGraphicsComponent::~ZGraphicsComponent()
{
    shaders_.clear();
    materials_.clear();
}

void ZGraphicsComponent::Initialize(std::shared_ptr<ZModel> model, std::shared_ptr<ZShader> shader)
{
    modelObject_ = model;
    if (shader) currentShaderObject_ = shader;
}

void ZGraphicsComponent::Initialize(std::shared_ptr<ZOFNode> root)
{
    ZComponent::Initialize();

    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        zenith::Log("Could not initalize ZGraphicsComponent", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("activeShader") != props.end() && props["activeShader"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> activeShaderProp = props["activeShader"]->Value<ZOFNumber>(0);
        activeShaderIndex_ = activeShaderProp->value;
    }

    if (props.find("highlightColor") != props.end() && props["highlightColor"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> hColorProp = props["highlightColor"]->Value<ZOFNumberList>(0);
        highlightColor_ = glm::vec4(hColorProp->value[0], hColorProp->value[1], hColorProp->value[2], 1.f);
    }

    if (props.find("highlightShader") != props.end() && props["highlightShader"]->HasValues())
    {
        std::shared_ptr<ZOFString> hShaderProp = props["highlightShader"]->Value<ZOFString>(0);
        if (zenith::Graphics()->Shaders().find(hShaderProp->value) != zenith::Graphics()->Shaders().end())
        {
            highlightShader_ = zenith::Graphics()->Shaders()[hShaderProp->value];
        }
    }

    if (props.find("shaders") != props.end() && props["shaders"]->HasValues())
    {
        std::shared_ptr<ZOFStringList> shadersProp = props["shaders"]->Value<ZOFStringList>(0);
        for (std::string id : shadersProp->value)
        {
            shaders_.push_back(id);
        }
    }

    if (props.find("billboard") != props.end() && props["billboard"]->HasValues())
    {
        std::shared_ptr<ZOFString> billboardProp = props["billboard"]->Value<ZOFString>(0);
        isBillboard_ = billboardProp->value == "Yes";
    }

    if (props.find("instances") != props.end() && props["instances"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> instancesProp = props["instances"]->Value<ZOFNumber>(0);
        instanceData_.count = instancesProp->value;
        for (unsigned int i = 0; i < instanceData_.count; i++)
        {
            glm::mat4 world(1.f);
            auto range = std::floor((i * 2 - 1) / 2);
            auto x = -range + (std::rand() % static_cast<int>(range * 2 + 1));
            auto z = -range + (std::rand() % static_cast<int>(range * 2 + 1));
            glm::translate(world, glm::vec3(x, 0.f, z));
            instanceData_.translations.push_back(world);
        }
    }

    if (props.find("model") != props.end() && props["model"]->HasValues())
    {
        std::shared_ptr<ZOFString> nameProp = props["model"]->Value<ZOFString>(0);
        if (nameProp->value.find("ZMOD") != std::string::npos)
        {
            model_ = nameProp->value;
        }
        else
        {
            if (props["model"]->ValueCount() > 1)
            {
                std::shared_ptr<ZOFNumberList> scaleProp = props["model"]->Value<ZOFNumberList>(1);
                modelObject_ = zenith::GraphicsFactory()->CreateModel(nameProp->value, glm::vec3(scaleProp->value[0], scaleProp->value[1], scaleProp->value[2]));
            }
            else
            {
                modelObject_ = zenith::GraphicsFactory()->CreateModel(nameProp->value);
            }
            modelObject_->SetInstanceData(instanceData_);
        }
    }

    // If there are any material subcomponents for this graphics component, we parse them with this loop
    for (ZOFChildMap::iterator matIt = node->children.begin(); matIt != node->children.end(); matIt++)
    {
        if (matIt->first == "Material")
        {
// TODO: Move material creation out of graphics component.
// We should be able to reuse materials across different objects.
            std::shared_ptr<ZMaterial> material = std::make_shared<ZMaterial>();
            material->Initialize(matIt->second);
            materials_.push_back(material);
        }
    }
    if (materials_.empty()) materials_.push_back(ZMaterial::DefaultMaterialPBR());
}

std::shared_ptr<ZComponent> ZGraphicsComponent::Clone()
{
    std::shared_ptr<ZGraphicsComponent> clone = std::make_shared<ZGraphicsComponent>();
    clone->activeShaderIndex_ = activeShaderIndex_;
    clone->shaders_ = shaders_;
    clone->model_ = model_;
    clone->modelObject_ = modelObject_;
    clone->currentShaderObject_ = currentShaderObject_;
    clone->materials_ = materials_;
    clone->highlightShader_ = highlightShader_;
    clone->highlightColor_ = highlightColor_;
    clone->instanceData_ = instanceData_;
    clone->isBillboard_ = isBillboard_;
    return clone;
}

void ZGraphicsComponent::Render(ZRenderOp renderOp)
{
    if (gameCamera_ == nullptr || modelObject_ == nullptr) return;

    std::shared_ptr<ZCameraComponent> cameraComp = gameCamera_->FindComponent<ZCameraComponent>();

    glm::mat4 modelMatrix = object_->Scene()->TopMatrix();
    glm::mat4 projectionMatrix = cameraComp->ProjectionMatrix();
    glm::mat4 viewMatrix = cameraComp->ViewMatrix();

    std::shared_ptr<ZShader> shader;
    if (renderOp == ZRenderOp::Shadow)
    {
        shader = zenith::Graphics()->ShadowShader();
    }
    else if (renderOp == ZRenderOp::Depth)
    {
        shader = zenith::Graphics()->DepthShader();
    }
    else
    {
        shader = ActiveShader();
    }

    // Make sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
    zenith::Graphics()->Strategy()->EnableStencilBuffer();

    if (shader)
    {
        shader->Use(gameLights_);

        if (renderOp == ZRenderOp::Color)
        {
            zenith::Graphics()->Strategy()->BindTexture(zenith::Graphics()->DepthBuffer(), 0);
            zenith::Graphics()->Strategy()->BindTexture(zenith::Graphics()->ShadowBuffer(), 1);
            shader->SetInt("depthTexture", 0);
            shader->SetInt("shadowTexture", 1);
        }

        shader->SetMat4("P", projectionMatrix);
        shader->SetMat4("V", viewMatrix);
        shader->SetMat4("M", modelMatrix);
        shader->SetMat4("ViewProjection", object_->Scene()->ViewProjection());
        shader->SetMat4("P_lightSpace", zenith::Graphics()->LightSpaceMatrix());
        shader->SetVec3("viewPosition", gameCamera_->Position());

        if (object_->Scene()->Skybox() != nullptr)
        {
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

std::shared_ptr<ZShader> ZGraphicsComponent::ActiveShader()
{
    if (currentShaderObject_) return currentShaderObject_;
    if (shaders_.empty()) return nullptr;

    if (zenith::Graphics()->Shaders().find(shaders_[activeShaderIndex_]) != zenith::Graphics()->Shaders().end())
    {
        currentShaderObject_ = zenith::Graphics()->Shaders()[shaders_[activeShaderIndex_]];
        return currentShaderObject_;
    }
    return nullptr;
}

std::shared_ptr<ZModel> ZGraphicsComponent::Model()
{
    if (modelObject_) return modelObject_;

    if (zenith::Graphics()->Models().find(model_) != zenith::Graphics()->Models().end())
    {
        modelObject_ = zenith::Graphics()->Models()[model_];
        modelObject_->SetInstanceData(instanceData_);
        return modelObject_;
    }
    return nullptr;
}

void ZGraphicsComponent::AddMaterial(std::shared_ptr<ZMaterial> material)
{
    materials_.push_back(material);
}

void ZGraphicsComponent::SetOutline(glm::vec4 color)
{
    if (highlightShader_ == nullptr)
    {
        highlightShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/outline.frag"));
        highlightShader_->Initialize();
    }
    highlightColor_ = color;
}

void ZGraphicsComponent::DrawOutlineIfEnabled(glm::mat4& model, glm::mat4& viewProjection)
{
    if (highlightShader_ == nullptr) return;

    zenith::Graphics()->Strategy()->DisableStencilBuffer();
    highlightShader_->Activate();

    glm::mat4 highlightModelMatrix = glm::scale(model, glm::vec3(1.03f, 1.03f, 1.03f));

    highlightShader_->SetMat4("ViewProjection", viewProjection);
    highlightShader_->SetMat4("M", highlightModelMatrix);
    highlightShader_->SetVec4("color", highlightColor_);

    modelObject_->Render(highlightShader_.get(), materials_);

    zenith::Graphics()->Strategy()->EnableStencilBuffer();
}

void ZGraphicsComponent::ClearOutline()
{
    if (highlightShader_ != nullptr) highlightShader_ = nullptr;
}
