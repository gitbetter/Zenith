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

std::map<std::string, ZLightType> ZLight::lightTypesMap = {
    {"Directional", ZLightType::Directional},
    {"Point", ZLightType::Point},
    {"Spot", ZLightType::Spot},
    {"Hemisphere", ZLightType::Hemisphere}
};

ZLight::ZLight(ZLightType lightType) : ZGameObject(glm::vec3(1.f))
{
    type = lightType; enabled = true;
    id_ = "ZLT_" + zenith::IDSequence()->Next();
}

void ZLight::Initialize(std::shared_ptr<ZOFNode> root)
{
    ZGameObject::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        zenith::Log("Could not initalize ZLight", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("type") != props.end() && props["type"]->HasValues())
    {
        std::shared_ptr<ZOFString> prop = props["type"]->Value<ZOFString>(0);
        type = lightTypesMap[prop->value];
    }

    if (props.find("ambient") != props.end() && props["ambient"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["ambient"]->Value<ZOFNumberList>(0);
        ambient = glm::vec3(prop->value[0], prop->value[1], prop->value[2]);
    }

    if (props.find("color") != props.end() && props["color"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["color"]->Value<ZOFNumberList>(0);
        color = glm::vec3(prop->value[0], prop->value[1], prop->value[2]);
    }

    if (props.find("constantAttenuation") != props.end() && props["constantAttenuation"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["constantAttenuation"]->Value<ZOFNumber>(0);
        attenuation.constant = prop->value;
    }

    if (props.find("linearAttenuation") != props.end() && props["linearAttenuation"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["linearAttenuation"]->Value<ZOFNumber>(0);
        attenuation.linear = prop->value;
    }

    if (props.find("quadraticProp") != props.end() && props["quadraticProp"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["quadraticProp"]->Value<ZOFNumber>(0);
        attenuation.quadratic = prop->value;
    }

    if (props.find("spotConeDirection") != props.end() && props["spotConeDirection"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["spotConeDirection"]->Value<ZOFNumberList>(0);
        spot.coneDirection = glm::vec3(prop->value[0], prop->value[1], prop->value[2]);
    }

    if (props.find("spotCutoff") != props.end() && props["spotCutoff"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["spotCutoff"]->Value<ZOFNumber>(0);
        spot.cutoff = prop->value;
    }

    if (props.find("spotExponent") != props.end() && props["spotExponent"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> prop = props["spotExponent"]->Value<ZOFNumber>(0);
        spot.exponent = prop->value;
    }

    if (props.find("hemisphereSky") != props.end() && props["hemisphereSky"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["hemisphereSky"]->Value<ZOFNumberList>(0);
        hemisphere.skyColor = glm::vec3(prop->value[0], prop->value[1], prop->value[2]);
    }

    if (props.find("hemisphereGround") != props.end() && props["hemisphereGround"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> prop = props["hemisphereGround"]->Value<ZOFNumberList>(0);
        hemisphere.groundColor = glm::vec3(prop->value[0], prop->value[1], prop->value[2]);
    }
}

std::shared_ptr<ZGameObject> ZLight::Clone()
{
    std::shared_ptr<ZLight> clone = std::make_shared<ZLight>(type);

    clone->id_ = id_;
    clone->enabled = enabled;
    clone->ambient = ambient;
    clone->color = color;
    clone->attenuation = attenuation;

    clone->SetPosition(Position());
    clone->SetOrientation(Orientation());
    clone->SetScale(Scale());

    if (clone->type == ZLightType::Spot)
        clone->spot = spot;
    else if (clone->type == ZLightType::Hemisphere)
        clone->hemisphere = hemisphere;

    return clone;
}
