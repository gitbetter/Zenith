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
#include "ZAssets.hpp"
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

ZIDSequence ZGraphicsComponent::idGenerator_;

ZGraphicsComponent::ZGraphicsComponent() : ZComponent()
{
    name = "GraphicsComponent_" + std::to_string(idGenerator_.Next());
}

void ZGraphicsComponent::Initialize(const ZHModel& model)
{
    if (model)
    {
        AddModel(model);
    }

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.SetRenderLayer(ZAssets::GameObjectManager()->RenderLayer(rootObject));
    overrideState = writer.End();
}

void ZGraphicsComponent::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (!dataNode)
    {
        LOG("Could not initalize ZGraphicsComponent", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("highlightColor") != props.end() && props["highlightColor"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> hColorProp = props["highlightColor"]->Value<ZOFNumberList>(0);
        SetOutline(glm::vec4(hColorProp->value[0], hColorProp->value[1], hColorProp->value[2], 1.f));
    }


    if (props.find("instances") != props.end() && props["instances"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> instancesProp = props["instances"]->Value<ZOFNumber>(0);
        instanceData.count = instancesProp->value;
        instanceData.translations.clear();
    }

    if (props.find("models") != props.end() && props["models"]->HasValues())
    {
        std::shared_ptr<ZOFHandleList> modelProp = props["models"]->Value<ZOFHandleList>(0);
        for (unsigned int handle : modelProp->value)
        {
            AddModel(ZHModel(handle));
        }
    }

    if (props.find("materials") != props.end() && props["materials"]->HasValues())
    {
        std::shared_ptr<ZOFHandleList> materialsProp = props["materials"]->Value<ZOFHandleList>(0);
        for (unsigned int handle : materialsProp->value)
        {
            materials.push_back(ZHMaterial(handle));
        }
    }

    if (props.find("billboard") != props.end() && props["billboard"]->HasValues())
    {
        std::shared_ptr<ZOFString> billboardProp = props["billboard"]->Value<ZOFString>(0);
        isBillboard = billboardProp->value == "Yes";
    }

    if (props.find("isShadowCaster") != props.end() && props["isShadowCaster"]->HasValues())
    {
        std::shared_ptr<ZOFString> shadowCasterProp = props["isShadowCaster"]->Value<ZOFString>(0);
        isShadowCaster = shadowCasterProp->value == "Yes";
    }

    if (props.find("hasDepthInfo") != props.end() && props["hasDepthInfo"]->HasValues())
    {
        std::shared_ptr<ZOFString> depthInfoProp = props["hasDepthInfo"]->Value<ZOFString>(0);
        hasDepthInfo = depthInfoProp->value == "Yes";
    }
}

void ZGraphicsComponent::OnCloned(const ZHComponent& original)
{
    ZGraphicsComponent* originalObj = ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(original);
    models = originalObj->models;
    materials = originalObj->materials;
    outlineMaterial = originalObj->outlineMaterial;
    instanceData = originalObj->instanceData;
    isBillboard = originalObj->isBillboard;
}

void ZGraphicsComponent::OnUpdate(double deltaTime)
{
    for (const ZHModel& model : models)
    {
        if (model.IsNull())
        {
            return;
        }

        auto scene = ZAssets::GameObjectManager()->Scene(rootObject);
        if (!scene || !gameCamera)
        {
            return;
        }

        if (hasAABB && bounds.IsValid())
        {
            scene->AddBVHPrimitive(ZBVHPrimitive(rootObject, bounds));
        }

        glm::mat4 modelMatrix = ZAssets::GameObjectManager()->ModelMatrix(rootObject);

        auto viewPos = ZAssets::GameObjectManager()->Position(rootObject) - ZAssets::GameObjectManager()->Position(gameCamera);

        ZRenderStateGroupWriter writer(overrideState);
        writer.Begin();
        if (hasDepthInfo)
        {
            writer.SetRenderDepth(viewPos.z * 100.f);
        }
        if (ZAssets::GameObjectManager()->RenderLayer(rootObject) == ZRenderLayer::UI)
        {
            writer.SetRenderDepth(0.f);
            writer.SetDepthStencilState({ ZDepthStencilState::None });
        }
        else if (outlineMaterial)
        {
            writer.SetDepthStencilState({ ZDepthStencilState::Stencil, ZDepthStencilState::Depth });
        }
        overrideState = writer.End();

        std::shared_ptr<ZRenderStateGroup> cameraState = ZAssets::GameObjectManager()->RenderState(gameCamera);
        std::shared_ptr<ZRenderStateGroup> objectState = ZAssets::GameObjectManager()->RenderState(rootObject);
        std::shared_ptr<ZRenderStateGroup> modelState = ZAssets::ModelManager()->RenderState(model);
        std::shared_ptr<ZRenderStateGroup> skyboxState = nullptr;

        if (scene->Skybox())
        {
            skyboxState = ZAssets::GameObjectManager()->RenderState(scene->Skybox());
        }

        if (materials.empty())
        {
            materials.push_back(ZAssets::MaterialManager()->Default());
        }

        // This setup assumes a forward rendering pipeline
        for (const auto& mesh : ZAssets::ModelManager()->Meshes(model))
        {
            ZPR_SESSION_COLLECT_VERTICES(mesh.Vertices().size());

            auto meshState = mesh.renderState;

            ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::Triangle);

            if (hasDepthInfo)
            {
                auto depthTask = ZRenderTask::Compile(drawCall,
                    { cameraState, objectState, modelState, meshState, overrideState },
                    ZRenderPass::Depth()
                );
                depthTask->Submit({ ZRenderPass::Depth() });
            }

            if (isShadowCaster)
            {
                auto shadowTask = ZRenderTask::Compile(drawCall,
                    { cameraState, objectState, modelState, meshState, overrideState },
                    ZRenderPass::Shadow()
                );
                shadowTask->Submit({ ZRenderPass::Shadow() });
            }

            if (hasLightingInfo)
            {
                for (const auto& light : gameLights)
                {
                    auto lightState = ZAssets::GameObjectManager()->RenderState(light);
                    for (auto material : materials)
                    {
                        auto materialState = ZAssets::MaterialManager()->RenderState(material);

                        auto colorRenderTask = ZRenderTask::Compile(drawCall,
                            { cameraState, objectState, modelState, meshState, materialState, lightState, skyboxState, overrideState },
                            ZRenderPass::Color()
                        );
                        colorRenderTask->Submit({ ZRenderPass::Color() });
                    }
                }
            }
            else
            {
                for (auto material : materials)
                {
                    auto materialState = ZAssets::MaterialManager()->RenderState(material);

                    auto colorRenderTask = ZRenderTask::Compile(drawCall,
                        { cameraState, objectState, modelState, meshState, materialState, skyboxState, overrideState },
                        ZRenderPass::Color()
                    );
                    colorRenderTask->Submit({ ZRenderPass::Color() });
                }
            }
        }

        if (outlineMaterial)
        {
            UpdateOutlineDisplay(modelMatrix, model, cameraState);
        }

        if (scene->GameConfig().graphics.drawAABBDebug && hasAABB)
        {
            ZServices::Graphics()->DebugDraw(scene, bounds, glm::vec4(1.f));
        }
    }
}

void ZGraphicsComponent::UpdateOutlineDisplay(glm::mat4& modelMatrix, const ZHModel& model, const std::shared_ptr<ZRenderStateGroup>& cameraState)
{
    glm::mat4 highlightModelMatrix = glm::scale(modelMatrix, glm::vec3(1.03f, 1.03f, 1.03f));
    ZAssets::GameObjectManager()->SetModelMatrix(rootObject, highlightModelMatrix);

    ZRenderStateGroupWriter writer(overrideState);
    writer.Begin();
    writer.SetBlending(ZBlendMode::None);
    writer.SetDepthStencilState({ ZDepthStencilState::Depth });
    overrideState = writer.End();

    auto objectState = ZAssets::GameObjectManager()->RenderState(rootObject);
    auto modelState = ZAssets::ModelManager()->RenderState(model);

    for (const auto& mesh : ZAssets::ModelManager()->Meshes(model)) {
        auto meshState = mesh.renderState;
        auto materialState = ZAssets::MaterialManager()->RenderState(outlineMaterial);

        ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::Triangle);
        auto highlightRenderTask = ZRenderTask::Compile(drawCall,
            { cameraState, objectState, modelState, meshState, overrideState, materialState },
            ZRenderPass::Color()
        );
        highlightRenderTask->Submit({ ZRenderPass::Color() });
    }

    ZAssets::GameObjectManager()->SetModelMatrix(rootObject, modelMatrix);
}

void ZGraphicsComponent::AddMaterial(const ZHMaterial& material)
{
    if (material.IsNull())
    {
        return;
    }
    materials.push_back(material);
}

void ZGraphicsComponent::SetOutline(const glm::vec4& color)
{
    auto scene = ZAssets::GameObjectManager()->Scene(rootObject);
    if (!scene) return;

    if (!outlineMaterial.IsNull() && color == glm::vec4(0.f))
    {
        ZAssets::MaterialManager()->SetProperty(outlineMaterial, "albedo", color);
    }
    else if (outlineMaterial.IsNull())
    {
        ZMaterialProperties materialProperties;
        materialProperties.albedo = color;
        outlineMaterial = ZAssets::MaterialManager()->Create(materialProperties, ZAssets::ShaderManager()->Create("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/outline.frag"));
    }
}

void ZGraphicsComponent::AddModel(const ZHModel& model)
{
    models.push_back(model);
    ZAssets::ModelManager()->SetInstanceData(model, instanceData);
    SetupAABB();
}

void ZGraphicsComponent::SetupAABB()
{
    auto mat = !rootObject.IsNull() ? ZAssets::GameObjectManager()->ModelMatrix(rootObject) : glm::mat4(1.f);
    Transform(mat);
}

bool ZGraphicsComponent::IsVisible(ZFrustum frustrum)
{
    return !hasAABB || frustrum.Contains(bounds);
}

void ZGraphicsComponent::Transform(const glm::mat4& mat)
{
    if (!hasAABB || models.empty()) return;
    bounds = ZAABBox();
    for (ZHModel& model : models)
    {
        bounds = ZAABBox::Union(bounds, ZAssets::ModelManager()->Bounds(model));
    }
    bounds = mat * bounds;
}

void ZGraphicsComponent::ClearOutline()
{
    outlineMaterial = ZHMaterial();
}