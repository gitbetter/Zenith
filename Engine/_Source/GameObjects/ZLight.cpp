/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZLight.cpp

   Created by Adrian Sanchez on 01/02/2019.
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

#include "ZLight.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZScene.hpp"
#include "ZFrustum.hpp"
#include "ZCamera.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"

ZIDSequence ZLight::idGenerator_;
std::map<std::string, ZLightType> ZLight::lightTypesMap =
{
    {"Directional", ZLightType::Directional},
    {"Point", ZLightType::Point},
    {"Spot", ZLightType::Spot}
};

ZLight::ZLight(ZLightType lightType) : ZGameObject(), lightType(lightType)
{
    name = "Light_" + std::to_string(idGenerator_.Next());
    lightProperties.isEnabled = true;
}

void ZLight::OnCreate()
{
    lightProperties.lightType = static_cast<unsigned int>(lightType);
    lightProperties.direction = glm::vec4(glm::eulerAngles(ZAssets::GameObjectManager()->Orientation(handle)), 0.f);
    lightProperties.position = glm::vec4(ZAssets::GameObjectManager()->Position(handle), 1.f);

    lightspaceMatrices = std::vector<glm::mat4>(NUM_SHADOW_CASCADES, glm::mat4(1.f));

    uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Light, sizeof(ZLightUniforms));
    uniformBuffer->Update(offsetof(ZLightUniforms, light), sizeof(lightProperties), &lightProperties);
}

void ZLight::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (dataNode == nullptr)
    {
        LOG("Could not initalize ZLight", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("type") != props.end() && props["type"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["type"]->Value<ZOFString>(0);
        lightType = lightTypesMap[prop->value];
    }

    if (props.find("enabled") != props.end() && props["enabled"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["enabled"]->Value<ZOFString>(0);
        lightProperties.isEnabled = prop->value == "Yes";
    }

    if (props.find("ambient") != props.end() && props["ambient"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["ambient"]->Value<ZOFNumberList>(0);
        lightProperties.ambient = glm::vec4(prop->value[0], prop->value[1], prop->value[2], 1.f);
    }

    if (props.find("color") != props.end() && props["color"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["color"]->Value<ZOFNumberList>(0);
        lightProperties.color = glm::vec4(prop->value[0], prop->value[1], prop->value[2], 1.f);
    }

    if (props.find("constantAttenuation") != props.end() && props["constantAttenuation"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["constantAttenuation"]->Value<ZOFNumber>(0);
        lightProperties.constantAttenuation = prop->value;
    }

    if (props.find("linearAttenuation") != props.end() && props["linearAttenuation"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["linearAttenuation"]->Value<ZOFNumber>(0);
        lightProperties.linearAttenuation = prop->value;
    }

    if (props.find("quadraticProp") != props.end() && props["quadraticProp"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["quadraticProp"]->Value<ZOFNumber>(0);
        lightProperties.quadraticAttenuation = prop->value;
    }

    if (props.find("spotConeDirection") != props.end() && props["spotConeDirection"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["spotConeDirection"]->Value<ZOFNumberList>(0);
        lightProperties.coneDirection = glm::vec4(prop->value[0], prop->value[1], prop->value[2], 0.f);
    }

    if (props.find("spotCutoff") != props.end() && props["spotCutoff"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["spotCutoff"]->Value<ZOFNumber>(0);
        lightProperties.spotCutoff = prop->value;
    }

    if (props.find("spotExponent") != props.end() && props["spotExponent"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["spotExponent"]->Value<ZOFNumber>(0);
        lightProperties.spotExponent = prop->value;
    }
}

void ZLight::OnUpdate(double deltaTime)
{
    auto sceneSP = scene.lock();
    if (sceneSP == nullptr)
    {
        return;
    }

    ZCamera* camObject = ZAssets::GameObjectManager()->Dereference<ZCamera>(sceneSP->ActiveCamera());
    if (camObject != nullptr && camObject->isMoving)
    {
        UpdateLightspaceMatrices(camObject->frustum);
    }

    if (sceneSP->GameConfig().graphics.drawAABBDebug)
    {
        ZServices::Graphics()->DebugDraw(sceneSP, lightspaceRegion, glm::vec4(1.f));
    }
}

void ZLight::OnCloned(ZGameObject* original)
{
    if (ZLight* originalLight = dynamic_cast<ZLight*>(original))
    {
        lightType = originalLight->lightType;
        lightProperties = originalLight->lightProperties;

        ZAssets::GameObjectManager()->SetPosition(handle, ZAssets::GameObjectManager()->Position(originalLight->handle));
        ZAssets::GameObjectManager()->SetOrientation(handle, ZAssets::GameObjectManager()->Orientation(originalLight->handle));
        ZAssets::GameObjectManager()->SetScale(handle, ZAssets::GameObjectManager()->Scale(originalLight->handle));
    }
}

void ZLight::UpdateLightspaceMatrices(const ZFrustum& frustum)
{
    if (lightType == ZLightType::Directional)
    {
        shadowFarPlaneSplits = glm::vec4(frustum.far * 0.2f, frustum.far * 0.35f, frustum.far * 0.75f, frustum.far);
        lightspaceMatrices.clear();
        for (int i = 0; i < NUM_SHADOW_CASCADES; i++) {
            ZFrustum splitFrustum = frustum;
            splitFrustum.far = shadowFarPlaneSplits[i];
            splitFrustum.Recalculate();

            const glm::quat orientation = ZAssets::GameObjectManager()->Orientation(handle);
            glm::mat4 lightV = glm::lookAt(splitFrustum.center + glm::eulerAngles(glm::normalize(orientation)), splitFrustum.center, WORLD_UP);

            lightspaceRegion = ZAABBox();
            for (const auto& corner : splitFrustum.corners) {
                auto transformedCorner = glm::vec4(corner, 1.0) * lightV;
                lightspaceRegion = ZAABBox::Union(lightspaceRegion, transformedCorner);
            }

			glm::vec3 extents = glm::vec3(glm::abs(lightspaceRegion.maximum.x - lightspaceRegion.minimum.x),
                                          glm::abs(lightspaceRegion.maximum.y - lightspaceRegion.minimum.y),
                                          glm::abs(lightspaceRegion.maximum.z - lightspaceRegion.minimum.z)) * 0.5f;

            lightspaceMatrices.push_back(glm::ortho(-extents.x, extents.x, -extents.y, extents.y, -extents.z, extents.z) * lightV);
        }

        uniformBuffer->Update(offsetof(ZLightUniforms, ViewProjectionsLightSpace), sizeof(glm::mat4) * NUM_SHADOW_CASCADES, lightspaceMatrices.data());
        uniformBuffer->Update(offsetof(ZLightUniforms, shadowFarPlanes), sizeof(glm::vec4), glm::value_ptr(shadowFarPlaneSplits));
    }
}