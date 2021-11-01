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
#include "ZPlane.hpp"
#include "ZShader.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"

ZGrass::ZGrass(unsigned int instances) : 
    ZGameObject(), texture(ZHTexture()), instanceCount(instances), windDirection(1.f), windStrength(5.f), objectHeight(1.f), time_(0.f)
{
    ZServices::EventAgent()->Subscribe(this, &ZGrass::HandleTextureReady);
}

void ZGrass::OnCreate()
{
    graphicsComp_ = ZServices::ComponentManager()->CreateIn(ZComponentType::Graphics, handle);
    auto graphicsCompObj = ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(graphicsComp_);
    graphicsCompObj->Initialize(ZHModel());
    graphicsCompObj->isShadowCaster = false;
    graphicsCompObj->hasLightingInfo = false;

    uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::UserDefined, sizeof(ZGrassUniforms));
    uniformBuffer->Update(offsetof(ZGrassUniforms, windDirection), sizeof(windDirection), glm::value_ptr(windDirection));
    uniformBuffer->Update(offsetof(ZGrassUniforms, windStrength), sizeof(windStrength), &windStrength);
    uniformBuffer->Update(offsetof(ZGrassUniforms, objectHeight), sizeof(objectHeight), &objectHeight);

    std::vector<ZInstancedDataOptions> instanceDatas;
    for (unsigned int i = 0; i < cPolygonCount; i++)
    {
        ZHModel model = ZServices::ModelManager()->Create(ZModelType::Plane);
        UpdateVertexNormals(model);
        polygons_.push_back(model);

        ZInstancedDataOptions instanceData;
        instanceData.count = instanceCount;
        instanceDatas.push_back(instanceData);

        graphicsCompObj->AddModel(model);
    }

    auto length = static_cast<unsigned int>(std::sqrt(instanceCount));
    for (unsigned int i = 0; i < length; i++)
    {
        for (unsigned int j = 0; j < length; j++)
        {
            auto scale = ZServices::GameObjectManager()->Scale(handle);
            auto x = -(length * 0.5f) + i + (-2 + std::rand() % 4);
            auto z = -(length * 0.5f) + j + (-2 + std::rand() % 4);
            auto translation = ZServices::GameObjectManager()->Position(handle) + glm::vec3(scale.x * 2.f * x, scale.y * 2.f, scale.z * 2.f * z);
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

    auto leftover = instanceCount - (length * length);
    for (unsigned int i = 0; i < leftover; i++)
    {
        for (unsigned int k = 0; k < cPolygonCount; k++)
        {
            instanceDatas[k].translations.push_back(glm::mat4(0.f));
        }
    }

    for (unsigned int k = 0; k < cPolygonCount; k++)
    {
        ZServices::ModelManager()->SetInstanceData(polygons_[k], instanceDatas[k]);
    }
}

void ZGrass::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (dataNode == nullptr)
    {
        LOG("Could not initalize ZGrass", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("texture") != props.end() && props["texture"]->HasValues())
    {
        std::shared_ptr<ZOFHandle> textureProp = props["texture"]->Value<ZOFHandle>(0);
        texture = ZHTexture(textureProp->value);
    }

    if (props.find("instances") != props.end() && props["instances"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> instancesProp = props["instances"]->Value<ZOFNumber>(0);
        instanceCount = instancesProp->value;
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
        windDirection = glm::vec3(windDirectionProp->value[0], windDirectionProp->value[1], windDirectionProp->value[2]);
    }

    if (props.find("windStrength") != props.end() && props["windStrength"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> windStrengthProp = props["windStrength"]->Value<ZOFNumber>(0);
        windStrength = windStrengthProp->value;
    }
}

void ZGrass::OnUpdate(double deltaTime)
{
    auto sceneSP = scene.lock();
    if (sceneSP == nullptr)
    {
        return;
    }

    time_ += deltaTime;
    if (sceneSP->PlayState() == ZPlayState::Playing)
    {
        uniformBuffer->Update(offsetof(ZGrassUniforms, timestamp), sizeof(time_), &time_);
    }
}

void ZGrass::TrimPatch(const glm::vec3& position, const glm::vec3& size)
{
    for (unsigned int i = 0; i < cPolygonCount; i++)
    {
        auto polygon = polygons_[i];
        auto instanceData = ZServices::ModelManager()->InstanceData(polygon);
        instanceData.translations.erase(std::remove_if(instanceData.translations.begin(), instanceData.translations.end(), [position, size] (const glm::mat4& translation) {
            return translation[3][0] >= position.x && translation[3][0] <= position.x + size.x &&
                translation[3][2] >= position.z && translation[3][2] <= position.z + size.z;
        }), instanceData.translations.end());
        instanceData.count = instanceData.translations.size();
        ZServices::ModelManager()->SetInstanceData(polygon, instanceData);
    }
}

void ZGrass::UpdateVertexNormals(const ZHModel& model)
{
    ZMesh3DList meshes = ZServices::ModelManager()->Meshes(model);
    for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
    {
        auto vertices = (*meshIt).Vertices();
        for (auto vertexIt = vertices.begin(); vertexIt != vertices.end(); vertexIt++)
        {
            vertexIt->normal = glm::vec3(1.f, 1.f, 0.f);
        }
        (*meshIt).SetVertices(vertices);
    }
}

void ZGrass::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    if (event->Texture() == texture)
    {
        auto grassMaterial = ZServices::MaterialManager()->Create({ event->Texture() }, ZServices::ShaderManager()->Create("/Shaders/Vertex/grass.vert", "/Shaders/Pixel/blinnphong.frag"));
        ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(graphicsComp_)->AddMaterial(grassMaterial);
        ZServices::EventAgent()->Unsubscribe(this, &ZGrass::HandleTextureReady);
    }
}
