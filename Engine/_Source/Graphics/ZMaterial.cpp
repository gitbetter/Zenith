/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZMaterial.cpp

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

#include "ZServices.hpp"
#include "ZMaterial.hpp"
#include "ZTexture.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZMaterialReadyEvent.hpp"

ZIDSequence ZMaterial::idGenerator_("ZMAT");

ZMaterial::ZMaterial(int index)
{ 
    index_ = index;
    id_ = "ZMAT_" + idGenerator_.Next();
}

ZMaterial::ZMaterial(const std::vector<std::shared_ptr<ZTexture>>& textures)
    : ZMaterial(0)
{ 
    for (auto texture : textures)
    {
        textures_[texture->type] = texture;
    }
}

void ZMaterial::Initialize()
{
    if (!pendingTextures_.empty())
        ZServices::EventAgent()->Subscribe(this, &ZMaterial::HandleTextureReady);
    else
        ZServices::EventAgent()->Queue(std::make_shared<ZMaterialReadyEvent>(shared_from_this()));
}

void ZMaterial::SetProperty(const std::string& property, float value)
{
    if (property == "emission")
    {
        properties_.emission = value;
    }
    else if (property == "diffuse")
    {
        properties_.diffuse = value;
    }
    else if (property == "ambient")
    {
        properties_.ambient = value;
    }
    else if (property == "specular")
    {
        properties_.specular = value;
    }
    else if (property == "shininess")
    {
        properties_.shininess = value;
    }
    else if (property == "metallic")
    {
        properties_.metallic = value;
    }
    else if (property == "roughness")
    {
        properties_.roughness = value;
    }
    else if (property == "ao")
    {
        properties_.ao = value;
    }
    else if (property == "alpha")
    {
        properties_.alpha = value;
    }
    else if (property == "tiling")
    {
        properties_.tiling = value;
    }
}

void ZMaterial::SetProperty(const std::string& property, const glm::vec4& value)
{
    if (property == "albedo")
    {
        properties_.albedo = value;
    }
}

void ZMaterial::SetProperty(const std::string& property, bool value)
{
    if (property == "isPBR")
    {
        properties_.isPBR = value;
    }
    else if (property == "hasDisplacement")
    {
        properties_.hasDisplacement = value;
    }
}

std::shared_ptr<ZMaterial> ZMaterial::Default()
{
    static std::shared_ptr<ZMaterial> defaultMaterial = ZMaterial::CreateDefault();
    return defaultMaterial;
}

std::shared_ptr<ZMaterial> ZMaterial::CreateDefault()
{
    if (ZServices::Graphics()->HasPBR())
    {
        ZMaterialProperties materialProperties;
        materialProperties.albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.f);
        materialProperties.metallic = 0.1f;
        materialProperties.roughness = 0.75f;
        materialProperties.ao = 0.3f;
        materialProperties.isPBR = true;
        std::shared_ptr<ZMaterial> material = ZMaterial::Create(materialProperties);
        return material;
    }
    else
    {
        ZMaterialProperties materialProperties;
        materialProperties.albedo = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
        materialProperties.emission = 0.f;
        materialProperties.diffuse = 0.5f;
        materialProperties.ambient = 0.3f;
        materialProperties.specular = 0.2f;
        materialProperties.shininess = 0.2f;
        return ZMaterial::Create(materialProperties);
    }
}

void ZMaterial::Create(std::shared_ptr<ZOFTree> data, ZMaterialMap& outMaterialMap, const ZTextureMap& textureCache)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZMAT") == 0)
        {
            std::shared_ptr<ZOFObjectNode> materialNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            auto material = std::make_shared<ZMaterial>();
            material->id_ = it->first;

            for (ZOFPropertyMap::iterator it = materialNode->properties.begin(); it != materialNode->properties.end(); it++)
            {
                if (!it->second->HasValues()) continue;

                // Sets PBR flag based on present fields.
                if (!material->IsPBR())
                    material->SetProperty("isPBR", it->second->id == "metallic" || it->second->id == "roughness" || it->second->id == "ao");

                if (!material->HasDisplacement())
                    material->SetProperty("hasDisplacement", it->second->id == "height");

                // If a field is string based there may be textures associated with that field, otherwise
                // the material field is simple and programmatic.
                if (std::shared_ptr<ZOFString> strProp = it->second->Value<ZOFString>(0))
                {
                    if (textureCache.find(strProp->value) != textureCache.end())
                    {
                        ZTexture::ptr texture = textureCache.at(strProp->value);
                        material->AddTexture(it->second->id, texture);
                    }
                }
                else if (std::shared_ptr<ZOFNumber> numProp = it->second->Value<ZOFNumber>(0))
                {
                    material->SetProperty(it->second->id, numProp->value);
                }
                else if (std::shared_ptr<ZOFNumberList> numListProp = it->second->Value<ZOFNumberList>(0))
                {
                    material->SetProperty(it->second->id, glm::vec4(numListProp->value[0], numListProp->value[1], numListProp->value[2], 1.0));
                }
            }

            material->Initialize();

            outMaterialMap[material->ID()] = material;
        }
    }
}

void ZMaterial::CreateAsync(std::shared_ptr<ZOFTree> data, ZMaterialIDMap& outPendingMaterials, ZMaterialMap& outMaterials)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZMAT") == 0)
        {
            std::shared_ptr<ZOFObjectNode> materialNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            auto material = std::make_shared<ZMaterial>();
            material->id_ = it->first;

            for (ZOFPropertyMap::iterator matIt = materialNode->properties.begin(); matIt != materialNode->properties.end(); matIt++)
            {
                if (!matIt->second->HasValues()) continue;

                // Sets PBR flag based on present fields.
                if (!material->IsPBR())
                    material->SetProperty("isPBR", matIt->second->id == "metallic" || matIt->second->id == "roughness" || matIt->second->id == "ao");

                if (!material->HasDisplacement())
                    material->SetProperty("hasDisplacement", matIt->second->id == "height");

                // If a field is string based there may be textures associated with that field, otherwise
                // the material field is simple and programmatic.
                if (std::shared_ptr<ZOFString> strProp = matIt->second->Value<ZOFString>(0))
                {
                    material->AddTexture(matIt->second->id, strProp->value);
                }
                else if (std::shared_ptr<ZOFNumber> numProp = matIt->second->Value<ZOFNumber>(0))
                {
                    material->SetProperty(matIt->second->id, numProp->value);
                }
                else if (std::shared_ptr<ZOFNumberList> numListProp = matIt->second->Value<ZOFNumberList>(0))
                {
                    material->SetProperty(matIt->second->id, glm::vec4(numListProp->value[0], numListProp->value[1], numListProp->value[2], 1.0));
                }
            }

            if (material->pendingTextures_.empty()) {
                outMaterials[material->ID()] = material;
            }
            else {
                outPendingMaterials[material] = material->ID();
            }

            material->Initialize();
        }
    }
}

std::shared_ptr<ZMaterial> ZMaterial::Create(const ZMaterialProperties& materialProperties)
{
    auto material = std::make_shared<ZMaterial>(materialProperties);
    material->Initialize();
    return material;
}

std::shared_ptr<ZMaterial> ZMaterial::Create(const std::vector<std::shared_ptr<ZTexture>>& textures)
{
    auto material = std::make_shared<ZMaterial>(textures);
    material->Initialize();
    return material;
}

void ZMaterial::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    auto texture = event->Texture();
    if (!texture) return;

    auto it = pendingTextures_.begin();
    while (it != pendingTextures_.end()) {
        if (it->second == texture->name) {
            textures_[it->first] = texture;
            it = pendingTextures_.erase(it);
        }
        else {
            ++it;
        }
    }

    if (pendingTextures_.empty())
    {
        ZServices::EventAgent()->Unsubscribe(this, &ZMaterial::HandleTextureReady);
        ZServices::EventAgent()->Queue(std::make_shared<ZMaterialReadyEvent>(shared_from_this()));
    }
}
