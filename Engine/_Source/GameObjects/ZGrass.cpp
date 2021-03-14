/*
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZGrass.cpp

   Created by Adrian Sanchez on 09/08/2020.
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

#include "ZGrass.hpp"
#include "ZMaterial.hpp"
#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZAssetStore.hpp"
#include "ZPlane.hpp"
#include "ZShader.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"

ZGrass::ZGrass(unsigned int instances) : 
    ZGameObject(), textureId_(std::string()), instanceCount_(instances), windDirection_(1.f), windStrength_(5.f), time_(0.f), objectHeight_(1.f)
{
    ZServices::EventAgent()->Subscribe(this, &ZGrass::HandleTextureReady);
}

void ZGrass::Initialize()
{
    graphicsComp_ = std::static_pointer_cast<ZGraphicsComponent>(ZGraphicsComponent::CreateIn(shared_from_this()));
    graphicsComp_->Initialize();
    graphicsComp_->DisableShadowCasting();
    graphicsComp_->DisableLightingInfo();

    uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::UserDefined, sizeof(ZGrassUniforms));
    uniformBuffer_->Update(offsetof(ZGrassUniforms, windDirection), sizeof(windDirection_), glm::value_ptr(windDirection_));
    uniformBuffer_->Update(offsetof(ZGrassUniforms, windStrength), sizeof(windStrength_), &windStrength_);
    uniformBuffer_->Update(offsetof(ZGrassUniforms, objectHeight), sizeof(objectHeight_), &objectHeight_);

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();

    std::vector<ZInstancedDataOptions> instanceDatas;
    for (unsigned int i = 0; i < cPolygonCount; i++)
    {
        std::shared_ptr<ZModel> model = ZPlane::Create(glm::vec2(1.f));
        UpdateVertexNormals(model);
        polygons_.push_back(model);

        ZInstancedDataOptions instanceData;
        instanceData.count = instanceCount_;
        instanceDatas.push_back(instanceData);
    }

    auto length = static_cast<unsigned int>(std::sqrt(instanceCount_));
    for (unsigned int i = 0; i < length; i++)
    {
        for (unsigned int j = 0; j < length; j++)
        {
            auto scale = Scale();
            auto x = -(length * 0.5f) + i + (-2 + std::rand() % 4);
            auto z = -(length * 0.5f) + j + (-2 + std::rand() % 4);
            auto translation = Position() + glm::vec3(scale.x * 2.f * x, scale.y * 2.f, scale.z * 2.f * z);
            for (unsigned int k = 0; k < cPolygonCount; k++)
            {
                auto rotation = glm::quat(glm::vec3(glm::radians(-90.f), glm::radians(k == 2 ? 135.f : k * 45.f), glm::radians(0.f)));
                auto modelM = glm::translate(glm::mat4(1.f), translation) *
                    glm::toMat4(rotation) *
                    glm::scale(glm::mat4(1.f), scale);

                instanceDatas[k].translations.push_back(modelM);
            }
        }
    }

    auto leftover = instanceCount_ - (length * length);
    for (unsigned int i = 0; i < leftover; i++)
    {
        for (unsigned int k = 0; k < cPolygonCount; k++)
        {
            instanceDatas[k].translations.push_back(glm::mat4(0.f));
        }
    }

    for (unsigned int k = 0; k < cPolygonCount; k++)
    {
        polygons_[k]->SetInstanceData(instanceDatas[k]);
    }

    ZGameObject::Initialize();
}

void ZGrass::Initialize(std::shared_ptr<ZOFNode> root)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZGrass", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    std::shared_ptr<ZShader> grassShader = nullptr;

    if (props.find("texture") != props.end() && props["texture"]->HasValues())
    {
        std::shared_ptr<ZOFString> textureProp = props["texture"]->Value<ZOFString>(0);
        textureId_ = textureProp->value;
    }

    if (props.find("instances") != props.end() && props["instances"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> instancesProp = props["instances"]->Value<ZOFNumber>(0);
        instanceCount_ = instancesProp->value;
    }

    if (props.find("shader") != props.end() && props["shader"]->HasValues())
    {
        std::shared_ptr<ZOFString> shaderProp = props["shader"]->Value<ZOFString>(0);
    }

    if (props.find("spacingStyle") != props.end() && props["spacingStyle"]->HasValues())
    {
        std::shared_ptr<ZOFString> spacingStyleProp = props["spacingStyle"]->Value<ZOFString>(0);
    }

    if (props.find("windDirection") != props.end() && props["windDirection"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> windDirectionProp = props["windDirection"]->Value<ZOFNumberList>(0);
        windDirection_ = glm::vec3(windDirectionProp->value[0], windDirectionProp->value[1], windDirectionProp->value[2]);
    }

    if (props.find("windStrength") != props.end() && props["windStrength"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> windStrengthProp = props["windStrength"]->Value<ZOFNumber>(0);
        windStrength_ = windStrengthProp->value;
    }

    ZGameObject::Initialize(root);
}

void ZGrass::Prepare(double deltaTime)
{
    auto scene = Scene();
    if (!scene) return;

    time_ += deltaTime;
    if (scene->PlayState() == ZPlayState::Playing) {
        uniformBuffer_->Update(offsetof(ZGrassUniforms, timestamp), sizeof(time_), &time_);
    }

    graphicsComp_->SetGameLights(scene->GameLights());
    graphicsComp_->SetGameCamera(scene->ActiveCamera());

    for (auto it = polygons_.begin(); it != polygons_.end(); it++)
    {
        graphicsComp_->SetModel(*it);
        graphicsComp_->Prepare(deltaTime, { renderState_ });
    }
}

void ZGrass::TrimPatch(const glm::vec3& position, const glm::vec3& size)
{
    for (unsigned int i = 0; i < cPolygonCount; i++)
    {
        auto polygon = polygons_[i];
        auto instanceData = polygon->InstanceData();
        instanceData.translations.erase(std::remove_if(instanceData.translations.begin(), instanceData.translations.end(), [position, size] (const glm::mat4& translation) {
            return translation[3][0] >= position.x && translation[3][0] <= position.x + size.x &&
                translation[3][2] >= position.z && translation[3][2] <= position.z + size.z;
        }), instanceData.translations.end());
        instanceData.count = instanceData.translations.size();
        polygon->SetInstanceData(instanceData);
    }
}

void ZGrass::UpdateVertexNormals(std::shared_ptr<ZModel>& model)
{
    for (auto meshIt = model->Meshes().begin(); meshIt != model->Meshes().end(); meshIt++)
    {
        auto vertices = meshIt->second->Vertices();
        for (auto vertexIt = vertices.begin(); vertexIt != vertices.end(); vertexIt++)
        {
            vertexIt->normal = glm::vec3(1.f, 1.f, 0.f);
        }
        meshIt->second->SetVertices(vertices);
    }
}

void ZGrass::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    if (event->Texture()->name == textureId_)
    {
        auto grassMaterial = ZMaterial::Create({ event->Texture() }, ZShader::Create("/Shaders/Vertex/grass.vert", "/Shaders/Pixel/blinnphong.frag"));
        graphicsComp_->AddMaterial(grassMaterial);
        ZServices::EventAgent()->Unsubscribe(this, &ZGrass::HandleTextureReady);
    }
}

DEFINE_OBJECT_CREATORS(ZGrass)
