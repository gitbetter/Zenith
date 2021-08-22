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
#include "ZScene.hpp"
#include "ZFrustum.hpp"
#include "ZCamera.hpp"
#include "ZUniformBuffer.hpp"
#include "ZRenderStateGroup.hpp"

std::map<std::string, ZLightType> ZLight::lightTypesMap = {
    {"Directional", ZLightType::Directional},
    {"Point", ZLightType::Point},
    {"Spot", ZLightType::Spot}
};

ZLight::ZLight(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) : ZGameObject(position, orientation, scale)
{
    properties.lightType = static_cast<unsigned int>(ZLightType::Point);
}

ZLight::ZLight(ZLightType lightType) : ZGameObject(glm::vec3(1.f))
{
    type = lightType;
    properties.isEnabled = true;
    id_ = "Light_" + std::to_string(idGenerator_.Next());
}

void ZLight::Initialize()
{
    ZGameObject::Initialize();

    properties.lightType = static_cast<unsigned int>(type);
    properties.direction = glm::vec4(glm::eulerAngles(Orientation()), 0.f);
    properties.position = glm::vec4(Position(), 1.f);

    lightspaceMatrices_ = std::vector<glm::mat4>(NUM_SHADOW_CASCADES, glm::mat4(1.f));

    uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::Light, sizeof(ZLightUniforms));
    uniformBuffer_->Update(offsetof(ZLightUniforms, light), sizeof(properties), &properties);

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();
}

void ZLight::Initialize(std::shared_ptr<ZOFNode> root)
{
    ZGameObject::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZLight", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("type") != props.end() && props["type"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["type"]->Value<ZOFString>(0);
        type = lightTypesMap[prop->value];
    }

    if (props.find("enabled") != props.end() && props["enabled"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["enabled"]->Value<ZOFString>(0);
        properties.isEnabled = prop->value == "Yes";
    }

    if (props.find("ambient") != props.end() && props["ambient"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["ambient"]->Value<ZOFNumberList>(0);
        properties.ambient = glm::vec4(prop->value[0], prop->value[1], prop->value[2], 1.f);
    }

    if (props.find("color") != props.end() && props["color"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["color"]->Value<ZOFNumberList>(0);
        properties.color = glm::vec4(prop->value[0], prop->value[1], prop->value[2], 1.f);
    }

    if (props.find("constantAttenuation") != props.end() && props["constantAttenuation"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["constantAttenuation"]->Value<ZOFNumber>(0);
        properties.constantAttenuation = prop->value;
    }

    if (props.find("linearAttenuation") != props.end() && props["linearAttenuation"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["linearAttenuation"]->Value<ZOFNumber>(0);
        properties.linearAttenuation = prop->value;
    }

    if (props.find("quadraticProp") != props.end() && props["quadraticProp"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["quadraticProp"]->Value<ZOFNumber>(0);
        properties.quadraticAttenuation = prop->value;
    }

    if (props.find("spotConeDirection") != props.end() && props["spotConeDirection"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["spotConeDirection"]->Value<ZOFNumberList>(0);
        properties.coneDirection = glm::vec4(prop->value[0], prop->value[1], prop->value[2], 0.f);
    }

    if (props.find("spotCutoff") != props.end() && props["spotCutoff"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["spotCutoff"]->Value<ZOFNumber>(0);
        properties.spotCutoff = prop->value;
    }

    if (props.find("spotExponent") != props.end() && props["spotExponent"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["spotExponent"]->Value<ZOFNumber>(0);
        properties.spotExponent = prop->value;
    }

    Initialize();
}

void ZLight::Prepare(double deltaTime)
{
    auto scene = Scene();
    if (!scene) return;

    auto cam = scene->ActiveCamera();
    if (cam && cam->Moving()) {
        UpdateLightspaceMatrices(cam->Frustum());
    }

    if (scene->GameConfig().graphics.drawAABBDebug) {
        ZServices::Graphics()->DebugDraw(scene, lightspaceRegion_, glm::vec4(1.f));
    }
}

std::shared_ptr<ZGameObject> ZLight::Clone()
{
    std::shared_ptr<ZLight> clone = ZLight::Create();
    clone->type = type;
    clone->id_ = id_;
    clone->properties = properties;

    clone->SetPosition(Position());
    clone->SetOrientation(Orientation());
    clone->SetScale(Scale());

    return clone;
}

void ZLight::UpdateLightspaceMatrices(const ZFrustum& frustum)
{
    if (type == ZLightType::Directional) {
        shadowFarPlaneSplits_ = glm::vec4(frustum.far * 0.2f, frustum.far * 0.35f, frustum.far * 0.75f, frustum.far);
        lightspaceMatrices_.clear();
        for (int i = 0; i < NUM_SHADOW_CASCADES; i++) {
            ZFrustum splitFrustum = frustum;
            splitFrustum.far = shadowFarPlaneSplits_[i];
            splitFrustum.Recalculate();

            glm::mat4 lightV = glm::lookAt(splitFrustum.center + glm::eulerAngles(glm::normalize(Orientation())), splitFrustum.center, WORLD_UP);

            lightspaceRegion_ = ZAABBox();
            for (const auto& corner : splitFrustum.corners) {
                auto transformedCorner = glm::vec4(corner, 1.0) * lightV;
                lightspaceRegion_ = ZAABBox::Union(lightspaceRegion_, transformedCorner);
            }

            glm::vec3 extents = glm::vec3(glm::abs(lightspaceRegion_.maximum.x - lightspaceRegion_.minimum.x),
                                          glm::abs(lightspaceRegion_.maximum.y - lightspaceRegion_.minimum.y),
                                          glm::abs(lightspaceRegion_.maximum.z - lightspaceRegion_.minimum.z)) * 0.5f;

            lightspaceMatrices_.push_back(glm::ortho(-extents.x, extents.x, -extents.y, extents.y, -extents.z, extents.z) * lightV);
        }

        uniformBuffer_->Update(offsetof(ZLightUniforms, ViewProjectionsLightSpace), sizeof(glm::mat4) * NUM_SHADOW_CASCADES, lightspaceMatrices_.data());
        uniformBuffer_->Update(offsetof(ZLightUniforms, shadowFarPlanes), sizeof(glm::vec4), glm::value_ptr(shadowFarPlaneSplits_));
    }
}

DEFINE_OBJECT_CREATORS(ZLight)