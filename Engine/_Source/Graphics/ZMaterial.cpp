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
#include "ZShader.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZMaterialReadyEvent.hpp"
#include "ZRenderStateGroup.hpp"
#include "ZUniformBuffer.hpp"

ZIDSequence ZMaterial::idGenerator_("ZMAT");

ZMaterial::ZMaterial()
{ 
    id_ = "ZMAT_" + std::to_string(idGenerator_.Next());
}

ZMaterial::ZMaterial(const ZMaterialProperties& materialProperties, const std::shared_ptr<ZShader>& shader) : ZMaterial()
{
    properties_ = materialProperties;
    shaderObject_ = shader;
}

ZMaterial::ZMaterial(const std::vector<std::shared_ptr<ZTexture>>& textures, const std::shared_ptr<ZShader>& shader) : ZMaterial()
{ 
    shaderObject_ = shader;
    for (auto texture : textures)
    {
        textures_[texture->type] = texture;
    }
}

void ZMaterial::Initialize()
{
    UpdateUniformMaterial();

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.SetBlending(ZBlendMode::Transluscent);
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();

    for (auto const& [key, val] : textures_)
        AddTexture(key, val);

    if (!pendingTextures_.empty())
        ZServices::EventAgent()->Subscribe(this, &ZMaterial::HandleTextureReady);    

    if (!shaderId_.empty()) {
        if (ZServices::AssetStore()->HasShader(shaderId_))
            SetShader(ZServices::AssetStore()->GetShader(shaderId_));
        else
            ZServices::EventAgent()->Subscribe(this, &ZMaterial::HandleShaderReady);
    }
    else if (shaderObject_) {
        SetShader(shaderObject_);
    }

    if (pendingTextures_.empty() && shaderObject_)
        ZServices::EventAgent()->Queue(std::make_shared<ZMaterialReadyEvent>(shared_from_this()));
}

void ZMaterial::SetShader(const std::shared_ptr<ZShader>& shader)
{
    if (!shader) return;

    shaderObject_ = shader;

    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.SetShader(shaderObject_);
    renderState_ = writer.End();
}

void ZMaterial::SetProperty(const std::string& property, float value)
{
    if (property == "emission")
    {
        properties_.look.emission = value;
        UpdateUniformMaterial();
    }
    else if (property == "diffuse")
    {
        properties_.look.diffuse = value;
        UpdateUniformMaterial();
    }
    else if (property == "ambient")
    {
        properties_.look.ambient = value;
        UpdateUniformMaterial();
    }
    else if (property == "specular")
    {
        properties_.look.specular = value;
        UpdateUniformMaterial();
    }
    else if (property == "shininess")
    {
        properties_.look.shininess = value;
        UpdateUniformMaterial();
    }
    else if (property == "metallic")
    {
        properties_.realisticLook.metallic = value;
        UpdateUniformMaterial();
    }
    else if (property == "roughness")
    {
        properties_.realisticLook.roughness = value;
        UpdateUniformMaterial();
    }
    else if (property == "ao")
    {
        properties_.realisticLook.ao = value;
        UpdateUniformMaterial();
    }
    else if (property == "alpha")
    {
        properties_.alpha = value;
        UpdateUniformMaterial();
    }
    else if (property == "tiling")
    {
        properties_.tiling = value;
        UpdateUniformMaterial();
    }
}

void ZMaterial::SetProperty(const std::string& property, const glm::vec4& value)
{
    if (property == "albedo")
    {
        properties_.look.albedo = value;
        UpdateUniformMaterial();
    }
}

void ZMaterial::SetProperty(const std::string& property, bool value)
{
    if (property == "isPBR")
    {
        properties_.isPBR = value;
        UpdateUniformMaterial();
    }
    else if (property == "hasDisplacement")
    {
        properties_.hasDisplacement = value;
        UpdateUniformMaterial();
    }
}

void ZMaterial::AddTexture(const std::string& slot, const std::shared_ptr<ZTexture>& texture)
{
    textures_[slot] = texture;

    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.BindTexture(texture);
    renderState_ = writer.End();
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
        materialProperties.look.albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.f);
        materialProperties.realisticLook.metallic = 0.1f;
        materialProperties.realisticLook.roughness = 0.75f;
        materialProperties.realisticLook.ao = 0.3f;
        materialProperties.isPBR = true;
        return ZMaterial::Create(materialProperties, ZServices::AssetStore()->PBRShader());
    }
    else
    {
        ZMaterialProperties materialProperties;
        materialProperties.look.albedo = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
        materialProperties.look.emission = 0.f;
        materialProperties.look.diffuse = 0.5f;
        materialProperties.look.ambient = 0.3f;
        materialProperties.look.specular = 0.2f;
        materialProperties.look.shininess = 0.2f;
        return ZMaterial::Create(materialProperties, ZServices::AssetStore()->BlinnPhongShader());
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

            for (ZOFPropertyMap::iterator matIt = materialNode->properties.begin(); matIt != materialNode->properties.end(); matIt++)
            {
                if (!matIt->second->HasValues()) continue;

                if (matIt->second->id == "shader") {
                    std::shared_ptr<ZOFString> shaderIdProp = matIt->second->Value<ZOFString>(0);
                    material->shaderId_ = shaderIdProp->value;
                    continue;
                }

                // Sets PBR flag based on present fields.
                if (!material->IsPBR())
                    material->SetProperty("isPBR", matIt->second->id == "metallic" || matIt->second->id == "roughness" || matIt->second->id == "ao");

                if (!material->HasDisplacement())
                    material->SetProperty("hasDisplacement", matIt->second->id == "height");

                // If a field is string based there may be textures associated with that field, otherwise
                // the material field is simple and programmatic.
                if (std::shared_ptr<ZOFString> strProp = matIt->second->Value<ZOFString>(0))
                {
                    if (textureCache.find(strProp->value) != textureCache.end())
                    {
                        ZTexture::ptr texture = textureCache.at(strProp->value);
                        material->textures_[matIt->second->id] = texture;
                    }
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

                if (matIt->second->id == "shader") {
                    std::shared_ptr<ZOFString> shaderIdProp = matIt->second->Value<ZOFString>(0);
                    material->shaderId_ = shaderIdProp->value;
                    continue;
                }

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

            if (material->pendingTextures_.empty() && material->shaderId_.empty()) {
                outMaterials[material->ID()] = material;
            }
            else {
                outPendingMaterials[material] = material->ID();
            }

            material->Initialize();
        }
    }
}

std::shared_ptr<ZMaterial> ZMaterial::Create(const ZMaterialProperties& materialProperties, const std::shared_ptr<ZShader>& shader)
{
    auto material = std::make_shared<ZMaterial>(materialProperties, shader);
    material->Initialize();
    return material;
}

std::shared_ptr<ZMaterial> ZMaterial::Create(const std::vector<std::shared_ptr<ZTexture>>& textures, const std::shared_ptr<ZShader>& shader)
{
    auto material = std::make_shared<ZMaterial>(textures, shader);
    material->Initialize();
    return material;
}

void ZMaterial::UpdateUniformMaterial()
{
    if (!uniformBuffer_)
        uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::Material, sizeof(ZMaterialUniforms));

    if (properties_.isPBR)
        uniformBuffer_->Update(offsetof(ZMaterialUniforms, pbrMaterial), sizeof(properties_.realisticLook), &properties_.realisticLook);
    else
        uniformBuffer_->Update(offsetof(ZMaterialUniforms, material), sizeof(properties_.look), &properties_.look);

    bool isTextured = !textures_.empty();
    uniformBuffer_->Update(offsetof(ZMaterialUniforms, isTextured), sizeof(isTextured), &isTextured);
    uniformBuffer_->Update(offsetof(ZMaterialUniforms, hasDisplacement), sizeof(properties_.hasDisplacement), &properties_.hasDisplacement);
}

void ZMaterial::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    auto texture = event->Texture();
    if (!texture) return;

    auto it = pendingTextures_.begin();
    while (it != pendingTextures_.end()) {
        if (it->second == texture->name) {
            AddTexture(it->first, texture);
            it = pendingTextures_.erase(it);
        }
        else {
            ++it;
        }
    }

    if (pendingTextures_.empty())
    {
        if (shaderObject_)
            ZServices::EventAgent()->Queue(std::make_shared<ZMaterialReadyEvent>(shared_from_this()));
        ZServices::EventAgent()->Unsubscribe(this, &ZMaterial::HandleTextureReady);
    }
}

void ZMaterial::HandleShaderReady(const std::shared_ptr<ZShaderReadyEvent>& event)
{
    auto shader = event->Shader();
    if (!shader) return;

    if (shader->Name() == shaderId_) {
        SetShader(shader);
    }

    if (shaderObject_)
    {
        if (pendingTextures_.empty())
            ZServices::EventAgent()->Queue(std::make_shared<ZMaterialReadyEvent>(shared_from_this()));
        ZServices::EventAgent()->Unsubscribe(this, &ZMaterial::HandleShaderReady);
    }
}
