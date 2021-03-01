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
#include "ZServices.hpp"
#include "ZAssetStore.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZScene.hpp"
#include "ZSkybox.hpp"
#include "ZCamera.hpp"
#include "ZOFTree.hpp"
#include "ZFrustum.hpp"

ZGraphicsComponent::ZGraphicsComponent() : ZComponent(), highlightColor_(0.f), isBillboard_(false)
{
    id_ = "ZCOMP_GRAPHICS_" + idGenerator_.Next();
}

ZGraphicsComponent::~ZGraphicsComponent()
{
    shadersIds_.clear();
    materials_.clear();
}

void ZGraphicsComponent::Initialize(std::shared_ptr<ZModel> model, std::shared_ptr<ZShader> shader)
{
    if (model)
    {
        SetModel(model);
    }
    if (shader)
    {
        currentShaderObject_ = shader;
    }
}

void ZGraphicsComponent::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZGraphicsComponent", ZSeverity::Error);
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
        highlightShaderId_ = hShaderProp->value;
    }

    if (props.find("shaders") != props.end() && props["shaders"]->HasValues())
    {
        std::shared_ptr<ZOFStringList> shadersProp = props["shaders"]->Value<ZOFStringList>(0);
        for (std::string id : shadersProp->value)
        {
            shadersIds_.push_back(id);
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
        instanceData_.translations.clear();
    }

    if (props.find("model") != props.end() && props["model"]->HasValues())
    {
        std::shared_ptr<ZOFString> nameProp = props["model"]->Value<ZOFString>(0);
        if (nameProp->value.find("ZMOD") != std::string::npos)
        {
            model_ = nameProp->value;
        }
    }

    if (props.find("materials") != props.end() && props["materials"]->HasValues())
    {
        std::shared_ptr<ZOFStringList> materialsProp = props["materials"]->Value<ZOFStringList>(0);
        for (const std::string& id : materialsProp->value)
        {
            materialIds_.push_back(id);
        }
    }
}

std::shared_ptr<ZComponent> ZGraphicsComponent::Clone()
{
    std::shared_ptr<ZGraphicsComponent> clone = std::make_shared<ZGraphicsComponent>();
    clone->activeShaderIndex_ = activeShaderIndex_;
    clone->shadersIds_ = shadersIds_;
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

void ZGraphicsComponent::Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp)
{
    auto model = Model();
    if (!gameCamera_ || !model) return;

    glm::mat4 modelMatrix = object_->ModelMatrix();
    glm::mat4 projectionMatrix = gameCamera_->ProjectionMatrix();
    glm::mat4 viewMatrix = gameCamera_->ViewMatrix();

    auto activeShader = shader;
    auto attachments = shader->Attachments();
    if (renderOp != ZRenderOp::Depth && renderOp != ZRenderOp::Shadow) {
        if (ActiveShader()) {
            activeShader = ActiveShader();
        }
    }
    activeShader->ClearAttachments();
    activeShader->SetAttachments(attachments);

    // Make sure we write to the stencil buffer (if outlining is enabled, we'll need these bits)
    ZServices::Graphics()->EnableStencilBuffer();
    ZServices::Graphics()->EnableAlphaBlending();

    if (activeShader)
    {
        activeShader->Activate();

        activeShader->BindAttachments();

        activeShader->Use(gameLights_);

        activeShader->SetMat4("P", projectionMatrix);
        activeShader->SetMat4("V", viewMatrix);
        activeShader->SetMat4("M", modelMatrix);
        activeShader->SetMat4("ViewProjection", object_->Scene()->ViewProjection());
        activeShader->SetVec3("viewPosition", gameCamera_->Position());
        activeShader->SetBool("instanced", false);

        if (!gameLights_.empty() && renderOp != ZRenderOp::Shadow) {
            // TODO: Defer this computation or choose light based on id for forward rendering
            auto light = gameLights_.begin()->second;
            activeShader->SetMat4List("ViewProjectionLightSpace", light->LightSpaceMatrices());
            activeShader->SetFloatList("shadowFarPlanes", light->ShadowFarPlaneSplits());
        }

        model->Render(activeShader, Materials());
    }

    ZServices::Graphics()->DisableAlphaBlending();
    DrawOutlineIfEnabled(modelMatrix, object_->Scene()->ViewProjection());

    if (object_->Scene()->GameConfig().graphics.drawAABBDebug && hasAABB_)
    {
        ZServices::Graphics()->DebugDraw(object_->Scene(), bounds_, glm::vec4(1.f));
    }
}

std::shared_ptr<ZShader> ZGraphicsComponent::ActiveShader()
{
    if (currentShaderObject_) return currentShaderObject_;

    auto scene = object_->Scene();
    if (!scene || shadersIds_.empty()) return nullptr;

    currentShaderObject_ = scene->AssetStore()->GetShader(shadersIds_[activeShaderIndex_]);
    return currentShaderObject_;
}

std::shared_ptr<ZModel> ZGraphicsComponent::Model()
{
    if (modelObject_) return modelObject_;
    
    auto scene = object_->Scene();
    if (!scene) return nullptr;

    if (scene->AssetStore()->HasModel(model_))
    {
        modelObject_ = scene->AssetStore()->GetModel(model_);
        modelObject_->SetInstanceData(instanceData_);
        SetupAABB();
        return modelObject_;
    }
    return nullptr;
}

const std::vector<std::shared_ptr<ZMaterial>>& ZGraphicsComponent::Materials()
{
    if (materialIds_.empty()) return materials_;

    auto it = materialIds_.begin();
    while (it != materialIds_.end())
    {
        if (object_->Scene() && object_->Scene()->AssetStore()->HasMaterial(*it))
        {
            auto material = object_->Scene()->AssetStore()->GetMaterial(*it);
            AddMaterial(material);
            it = materialIds_.erase(it);
        }
        else {
            ++it;
        }
    }
    return materials_;
}

void ZGraphicsComponent::AddMaterial(const std::shared_ptr<ZMaterial>& material)
{
    if (!material) return;
    materials_.push_back(material);
}

void ZGraphicsComponent::SetOutline(const glm::vec4& color)
{
    auto scene = object_->Scene();
    if (!scene) return;

    if (scene->AssetStore()->HasShader(highlightShaderId_)) {
        highlightShader_ = scene->AssetStore()->GetShader(highlightShaderId_);
    }
    else {
        highlightShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/outline.frag"));
        highlightShader_->Initialize();
    }
    highlightColor_ = color;
}

void ZGraphicsComponent::SetModel(const std::shared_ptr<ZModel>& model)
{
    modelObject_ = model;
    instanceData_ = modelObject_->InstanceData();
    SetupAABB();
}

void ZGraphicsComponent::SetupAABB()
{
    auto mat = object_ ? object_->ModelMatrix() : glm::mat4(1.f);
    Transform(mat);
}

bool ZGraphicsComponent::IsVisible(ZFrustum frustrum)
{
    return !hasAABB_ || (modelObject_ && frustrum.Contains(bounds_));
}

void ZGraphicsComponent::Transform(const glm::mat4& mat)
{
    auto model = Model();
    if (!hasAABB_ || !model) return;
    bounds_ = mat * model->Bounds();
}

void ZGraphicsComponent::DrawOutlineIfEnabled(const glm::mat4& worldMat, const glm::mat4& viewProjection)
{
    auto model = Model();
    if (!highlightShader_ || !model) return;

    ZServices::Graphics()->DisableStencilBuffer();
    highlightShader_->Activate();

    glm::mat4 highlightModelMatrix = glm::scale(worldMat, glm::vec3(1.03f, 1.03f, 1.03f));

    highlightShader_->SetMat4("ViewProjection", viewProjection);
    highlightShader_->SetMat4("M", highlightModelMatrix);
    highlightShader_->SetVec4("color", highlightColor_);

    model->Render(highlightShader_, Materials());

    ZServices::Graphics()->EnableStencilBuffer();
}

void ZGraphicsComponent::ClearOutline()
{
    if (highlightShader_ != nullptr) highlightShader_ = nullptr;
}

DEFINE_COMPONENT_CREATORS(ZGraphicsComponent)