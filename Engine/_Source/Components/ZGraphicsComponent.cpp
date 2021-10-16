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
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZMaterial.hpp"
#include "ZScene.hpp"
#include "ZSkybox.hpp"
#include "ZCamera.hpp"
#include "ZOFTree.hpp"
#include "ZFrustum.hpp"
#include "ZLight.hpp"
#include "ZRenderStateGroup.hpp"
#include "ZUniformBuffer.hpp"
#include "ZDrawCall.hpp"
#include "ZRenderTask.hpp"
#include "ZRenderPass.hpp"

#include <rttr/registration>

ZGraphicsComponent::ZGraphicsComponent() : ZComponent(), isBillboard_(false)
{
    id_ = "ZCOMP_GRAPHICS_" + std::to_string(idGenerator_.Next());
}

ZGraphicsComponent::~ZGraphicsComponent()
{
    materials_.clear();
}

void ZGraphicsComponent::Initialize(const ZHModel& model)
{
    if (model)
    {
        SetModel(model);
    }

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.SetRenderLayer(object_->RenderLayer());
    overrideState_ = writer.End();
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

    if (props.find("highlightColor") != props.end() && props["highlightColor"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> hColorProp = props["highlightColor"]->Value<ZOFNumberList>(0);
        SetOutline(glm::vec4(hColorProp->value[0], hColorProp->value[1], hColorProp->value[2], 1.f));
    }


    if (props.find("instances") != props.end() && props["instances"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> instancesProp = props["instances"]->Value<ZOFNumber>(0);
        instanceData_.count = instancesProp->value;
        instanceData_.translations.clear();
    }

    if (props.find("model") != props.end() && props["model"]->HasValues())
    {
        std::shared_ptr<ZOFHandle> modelProp = props["model"]->Value<ZOFHandle>(0);
        model_ = ZHModel(modelProp->value);
    }

    if (props.find("materials") != props.end() && props["materials"]->HasValues())
    {
        std::shared_ptr<ZOFHandleList> materialsProp = props["materials"]->Value<ZOFHandleList>(0);
        for (unsigned int handle : materialsProp->value)
        {
            materials_.push_back(ZHMaterial(handle));
        }
    }

    if (props.find("billboard") != props.end() && props["billboard"]->HasValues())
    {
        std::shared_ptr<ZOFString> billboardProp = props["billboard"]->Value<ZOFString>(0);
        isBillboard_ = billboardProp->value == "Yes";
    }

    if (props.find("isShadowCaster") != props.end() && props["isShadowCaster"]->HasValues())
    {
        std::shared_ptr<ZOFString> shadowCasterProp = props["isShadowCaster"]->Value<ZOFString>(0);
        isShadowCaster_ = shadowCasterProp->value == "Yes";
    }

    if (props.find("hasDepthInfo") != props.end() && props["hasDepthInfo"]->HasValues())
    {
        std::shared_ptr<ZOFString> depthInfoProp = props["hasDepthInfo"]->Value<ZOFString>(0);
        hasDepthInfo_ = depthInfoProp->value == "Yes";
    }
}

std::shared_ptr<ZComponent> ZGraphicsComponent::Clone()
{
    std::shared_ptr<ZGraphicsComponent> clone = std::make_shared<ZGraphicsComponent>();
    clone->model_ = model_;
    clone->materials_ = materials_;
    clone->outlineMaterial_ = outlineMaterial_;
    clone->instanceData_ = instanceData_;
    clone->isBillboard_ = isBillboard_;
    return clone;
}

void ZGraphicsComponent::Prepare(double deltaTime, const std::shared_ptr<ZRenderStateGroup>& additionalState)
{
	auto model = Model();
    if (model.IsNull())
    {
        return;
    }

    auto scene = object_->Scene();
    if (!scene || !gameCamera_)
    {
        return;
    }

    if (hasAABB_)
    {
        scene->AddBVHPrimitive(ZBVHPrimitive(object_->ID(), bounds_));
    }

    glm::mat4 modelMatrix = object_->ModelMatrix();

    auto viewPos = object_->Position() - gameCamera_->Position();

    ZRenderStateGroupWriter writer(overrideState_);
    writer.Begin();
    if (hasDepthInfo_)
    {
        writer.SetRenderDepth(viewPos.z * 100.f);
    }
    if (object_->RenderLayer() == ZRenderLayer::UI)
    {
        writer.SetRenderDepth(0.f);
        writer.SetDepthStencilState({ ZDepthStencilState::None });
    }
    else if (outlineMaterial_)
    {
        writer.SetDepthStencilState({ ZDepthStencilState::Stencil, ZDepthStencilState::Depth });
    }
    overrideState_ = writer.End();

    std::shared_ptr<ZRenderStateGroup> cameraState = gameCamera_->RenderState();
    std::shared_ptr<ZRenderStateGroup> objectState = object_->RenderState();
    std::shared_ptr<ZRenderStateGroup> modelState = ZServices::ModelManager()->RenderState(model);
    std::shared_ptr<ZRenderStateGroup> skyboxState = nullptr;

    if (scene->Skybox())
    {
        skyboxState = scene->Skybox()->RenderState();
    }

    if (materials_.empty())
    {
		materials_.push_back(ZServices::MaterialManager()->Default());
    }

    // This setup assumes a forward rendering pipeline
    for (const auto& mesh : ZServices::ModelManager()->Meshes(model))
    {
        ZPR_SESSION_COLLECT_VERTICES(mesh.Vertices().size());

        auto meshState = mesh.renderState;

        ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::Triangle);

        if (hasDepthInfo_)
        {
            auto depthTask = ZRenderTask::Compile(drawCall,
                { cameraState, objectState, modelState, meshState, additionalState, overrideState_ },
                ZRenderPass::Depth()
            );
            depthTask->Submit({ ZRenderPass::Depth() });
        }

        if (isShadowCaster_)
        {
            auto shadowTask = ZRenderTask::Compile(drawCall,
                { cameraState, objectState, modelState, meshState, additionalState, overrideState_ },
                ZRenderPass::Shadow()
            );
            shadowTask->Submit({ ZRenderPass::Shadow() });
        }

        if (hasLightingInfo_)
        {
            for (const auto& light : gameLights_)
            {
                auto lightState = light->RenderState();
                for (auto material : materials_)
                {
                    auto materialState = ZServices::MaterialManager()->RenderState(material);

                    auto colorRenderTask = ZRenderTask::Compile(drawCall,
                        { cameraState, objectState, modelState, meshState, additionalState, materialState, lightState, skyboxState, overrideState_ },
                        ZRenderPass::Color()
                    );
                    colorRenderTask->Submit({ ZRenderPass::Color() });
                }
            }
        }
        else
        {
            for (auto material : materials_)
            {
                auto materialState = ZServices::MaterialManager()->RenderState(material);

                auto colorRenderTask = ZRenderTask::Compile(drawCall,
                    { cameraState, objectState, modelState, meshState, additionalState, materialState, skyboxState, overrideState_ },
                    ZRenderPass::Color()
                );
                colorRenderTask->Submit({ ZRenderPass::Color() });
            }
        }
    }
    
    if (outlineMaterial_)
    {
        PrepareOutlineDisplay(modelMatrix, model, additionalState, cameraState);
    }

    if (object_->Scene()->GameConfig().graphics.drawAABBDebug && hasAABB_)
    {
        ZServices::Graphics()->DebugDraw(object_->Scene(), bounds_, glm::vec4(1.f));
    }
}

void ZGraphicsComponent::PrepareOutlineDisplay(glm::mat4& modelMatrix, const ZHModel& model, const std::shared_ptr<ZRenderStateGroup>& additionalState, const std::shared_ptr<ZRenderStateGroup>& cameraState)
{
    glm::mat4 highlightModelMatrix = glm::scale(modelMatrix, glm::vec3(1.03f, 1.03f, 1.03f));
    object_->SetModelMatrix(highlightModelMatrix);

    ZRenderStateGroupWriter writer(overrideState_);
    writer.Begin();
    writer.SetBlending(ZBlendMode::None);
    writer.SetDepthStencilState({ ZDepthStencilState::Depth });
    overrideState_ = writer.End();

    auto objectState = object_->RenderState();
    auto modelState = ZServices::ModelManager()->RenderState(model_);

    for (const auto& mesh : ZServices::ModelManager()->Meshes(model_)) {
        auto meshState = mesh.renderState;
        auto materialState = ZServices::MaterialManager()->RenderState(outlineMaterial_);

        ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::Triangle);
        auto highlightRenderTask = ZRenderTask::Compile(drawCall,
            { cameraState, objectState, modelState, meshState, additionalState, overrideState_, materialState },
            ZRenderPass::Color()
        );
        highlightRenderTask->Submit({ ZRenderPass::Color() });
    }

    object_->SetModelMatrix(modelMatrix);
}

void ZGraphicsComponent::AddMaterial(const ZHMaterial& material)
{
    if (!material) return;

    materials_.push_back(material);
}

void ZGraphicsComponent::SetOutline(const glm::vec4& color)
{
    auto scene = object_->Scene();
    if (!scene) return;

    if (!outlineMaterial_.IsNull() && color == glm::vec4(0.f))
    {
        ZServices::MaterialManager()->SetProperty(outlineMaterial_, "albedo", color);
    }
    else if (outlineMaterial_.IsNull())
    {
        ZMaterialProperties materialProperties;
        materialProperties.albedo = color;
        outlineMaterial_ = ZServices::MaterialManager()->Create(materialProperties, ZServices::ShaderManager()->Create("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/outline.frag"));
    }
}

void ZGraphicsComponent::SetModel(const ZHModel& model)
{
    model_ = model;
    ZServices::ModelManager()->SetInstanceData(model_, instanceData_);
    SetupAABB();
}

void ZGraphicsComponent::SetupAABB()
{
    auto mat = object_ ? object_->ModelMatrix() : glm::mat4(1.f);
    Transform(mat);
}

bool ZGraphicsComponent::IsVisible(ZFrustum frustrum)
{
    return !hasAABB_ || frustrum.Contains(bounds_);
}

void ZGraphicsComponent::Transform(const glm::mat4& mat)
{
    const ZHModel model = Model();
    if (!hasAABB_ || model.IsNull()) return;
    bounds_ = mat * ZServices::ModelManager()->Bounds(model);
}

void ZGraphicsComponent::ClearOutline()
{
    outlineMaterial_ = ZHMaterial();
}

DEFINE_COMPONENT_CREATORS(ZGraphicsComponent)

RTTR_REGISTRATION
{
    using namespace rttr;
    registration::class_<ZGraphicsComponent>("ZGraphicsComponent")
        .constructor<>()
        .property("Model", &ZGraphicsComponent::Model, &ZGraphicsComponent::SetModel)
        .property("Materials", &ZGraphicsComponent::Materials, &ZGraphicsComponent::SetMaterials)
        .property("HasAABB", &ZGraphicsComponent::HasAABB, &ZGraphicsComponent::SetHasAABB)
        .property("IsShadowCaster", &ZGraphicsComponent::IsShadowCaster, &ZGraphicsComponent::SetIsShadowCaster)
        .property("HasLightingInfo", &ZGraphicsComponent::HasLightingInfo, &ZGraphicsComponent::SetHasLightingInfo);
}