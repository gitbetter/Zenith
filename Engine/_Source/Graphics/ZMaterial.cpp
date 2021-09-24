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

ZIDSequence ZMaterialBase::idGenerator_;

ZMaterialBase::ZMaterialBase()
{ 
    name = "Material_" + std::to_string(idGenerator_.Next());
}

const std::string& ZMaterialManager::Name(const ZHMaterial& handle) const
{
	assert(!handle.IsNull() && "Cannot fetch property with a null material handle!");
	const ZMaterialBase* material = resourcePool_.Get(handle);
	return material->name;
}

const ZTextureMap& ZMaterialManager::Textures(const ZHMaterial& handle) const
{
	assert(!handle.IsNull() && "Cannot fetch property with a null material handle!");
	const ZMaterialBase* material = resourcePool_.Get(handle);
	return material->textures;
}

const ZHShader& ZMaterialManager::Shader(const ZHMaterial& handle) const
{
	assert(!handle.IsNull() && "Cannot fetch property with a null material handle!");
	const ZMaterialBase* material = resourcePool_.Get(handle);
	return material->shader;
}

float ZMaterialManager::Alpha(const ZHMaterial& handle) const
{
	assert(!handle.IsNull() && "Cannot fetch property with a null material handle!");
	const ZMaterialBase* material = resourcePool_.Get(handle);
	return material->properties.alpha;
}

bool ZMaterialManager::IsTextured(const ZHMaterial& handle) const
{
	assert(!handle.IsNull() && "Cannot fetch property with a null material handle!");
	const ZMaterialBase* material = resourcePool_.Get(handle);
	return !material->textures.empty();
}

bool ZMaterialManager::HasDisplacement(const ZHMaterial& handle) const
{
	assert(!handle.IsNull() && "Cannot fetch property with a null material handle!");
	const ZMaterialBase* material = resourcePool_.Get(handle);
	return material->hasDisplacement;
}

const std::shared_ptr<ZRenderStateGroup>& ZMaterialManager::RenderState(const ZHMaterial& handle) const
{
	assert(!handle.IsNull() && "Cannot fetch property with a null material handle!");
	const ZMaterialBase* material = resourcePool_.Get(handle);
	return material->renderState;
}

void ZMaterialManager::SetShader(const ZHMaterial& handle, const ZHShader& shader)
{
	assert(!handle.IsNull() && "Cannot set property with a null material handle!");
    ZMaterialBase* material = resourcePool_.Get(handle);
	material->shader = shader;

    ZRenderStateGroupWriter writer(material->renderState);
    writer.Begin();
    writer.SetShader(shader);
    material->renderState = writer.End();
}

void ZMaterialManager::SetAlpha(const ZHMaterial& handle, float alpha)
{
	assert(!handle.IsNull() && "Cannot set property with a null material handle!");
	ZMaterialBase* material = resourcePool_.Get(handle);
	material->properties.alpha = alpha;
}

void ZMaterialManager::SetProperty(const ZHMaterial& handle, const std::string& property, float value)
{
	assert(!handle.IsNull() && "Cannot set property with a null material handle!");
	ZMaterialBase* material = resourcePool_.Get(handle);

    if (property == "emission")
    {
        material->properties.emission = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "diffuse")
    {
        material->properties.diffuse = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "ambient")
    {
        material->properties.ambient = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "specular")
    {
        material->properties.specular = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "shininess")
    {
        material->properties.shininess = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "metallic")
    {
        material->properties.metallic = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "roughness")
    {
        material->properties.roughness = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "ao")
    {
        material->properties.ao = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "alpha")
    {
        material->properties.alpha = value;
        UpdateUniformMaterial(handle);
    }
    else if (property == "tiling")
    {
        material->properties.tiling = value;
        UpdateUniformMaterial(handle);
    }
}

void ZMaterialManager::SetProperty(const ZHMaterial& handle, const std::string& property, const glm::vec4& value)
{
	assert(!handle.IsNull() && "Cannot set property with a null material handle!");
	ZMaterialBase* material = resourcePool_.Get(handle);

    if (property == "albedo")
    {
        material->properties.albedo = value;
        UpdateUniformMaterial(handle);
    }
}

void ZMaterialManager::SetProperty(const ZHMaterial& handle, const std::string& property, bool value)
{
	assert(!handle.IsNull() && "Cannot set property with a null material handle!");
	ZMaterialBase* material = resourcePool_.Get(handle);

    if (property == "hasDisplacement")
    {
        material->hasDisplacement = value;
        UpdateUniformMaterial(handle);
    }
}

void ZMaterialManager::AddTexture(const ZHMaterial& handle, const std::string& slot, const ZHTexture& texture)
{
	assert(!handle.IsNull() && "Cannot set property with a null material handle!");
	ZMaterialBase* material = resourcePool_.Get(handle);

    material->textures[slot] = texture;

    ZRenderStateGroupWriter writer(material->renderState);
    writer.Begin();
    writer.BindTexture(texture);
    material->renderState = writer.End();

    UpdateUniformMaterial(handle);
}

ZHMaterial ZMaterialManager::Default()
{
    static ZHMaterial defaultMaterial = CreateDefault();
    return defaultMaterial;
}

ZHMaterial ZMaterialManager::CreateDefault()
{
	ZMaterialProperties materialProperties;
	materialProperties.albedo = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
	materialProperties.emission = 0.f;
	materialProperties.diffuse = 0.5f;
	materialProperties.ambient = 0.3f;
	materialProperties.specular = 0.2f;
	materialProperties.shininess = 0.2f;
	materialProperties.metallic = 0.1f;
	materialProperties.roughness = 0.75f;
	materialProperties.ao = 0.3f;
	return Create(materialProperties, ZServices::ShaderManager()->BlinnPhongShader());
}

ZHMaterial ZMaterialManager::Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
    if (dataNode->type != ZOFObjectType::Material)
    {
        return ZHMaterial();
    }

	ZHMaterial handle(dataHandle.value);

	ZMaterialBase* material = resourcePool_.New(handle);

	if (dataNode->properties.find("albedo") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumberList> prop = dataNode->properties["albedo"]->Value<ZOFNumberList>(0);
		SetProperty(handle, "albedo", glm::vec4(prop->value[0], prop->value[1], prop->value[2], 1.0));
	}

	if (dataNode->properties.find("alpha") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["albedo"]->Value<ZOFNumber>(0);
		SetProperty(handle, "alpha", prop->value);
	}

	if (dataNode->properties.find("tiling") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["tiling"]->Value<ZOFNumber>(0);
		SetProperty(handle, "tiling", prop->value);
	}

	if (dataNode->properties.find("emission") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["emission"]->Value<ZOFNumber>(0);
		SetProperty(handle, "emission", prop->value);
	}

	if (dataNode->properties.find("ambient") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["ambient"]->Value<ZOFNumber>(0);
		SetProperty(handle, "ambient", prop->value);
	}

	if (dataNode->properties.find("diffuse") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["diffuse"]->Value<ZOFNumber>(0);
		SetProperty(handle, "diffuse", prop->value);
	}

	if (dataNode->properties.find("specular") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["specular"]->Value<ZOFNumber>(0);
		SetProperty(handle, "specular", prop->value);
	}

	if (dataNode->properties.find("shininess") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["shininess"]->Value<ZOFNumber>(0);
		SetProperty(handle, "shininess", prop->value);
	}

	if (dataNode->properties.find("metallic") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["metallic"]->Value<ZOFNumber>(0);
		SetProperty(handle, "metallic", prop->value);
	}

	if (dataNode->properties.find("roughness") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["roughness"]->Value<ZOFNumber>(0);
		SetProperty(handle, "roughness", prop->value);
	}

	if (dataNode->properties.find("ao") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["ao"]->Value<ZOFNumber>(0);
		SetProperty(handle, "ao", prop->value);
	}

	if (dataNode->properties.find("shader") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFHandle> shaderProp = dataNode->properties["shader"]->Value<ZOFHandle>(0);
		SetShader(handle, ZHShader(shaderProp->value));
	}

	if (dataNode->properties.find("height") != dataNode->properties.end())
	{
		SetProperty(handle, "hasDisplacement", true);
	}

	if (dataNode->properties.find("name") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> prop = dataNode->properties["name"]->Value<ZOFString>(0);
		material->name = prop->value;
	}

	Track(handle);

	return handle;
}

void ZMaterialManager::DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
    if (dataNode->type != ZOFObjectType::Material)
    {
        return;
    }

	ZHMaterial handle(dataHandle.value);

	ZMaterialBase* material = resourcePool_.New(handle);

	if (dataNode->properties.find("albedo") != dataNode->properties.end())
	{
        std::shared_ptr<ZOFNumberList> prop = dataNode->properties["albedo"]->Value<ZOFNumberList>(0);
		SetProperty(handle, "albedo", glm::vec4(prop->value[0], prop->value[1], prop->value[2], 1.0));
	}

	if (dataNode->properties.find("alpha") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["albedo"]->Value<ZOFNumber>(0);
		SetProperty(handle, "alpha", prop->value);
	}

	if (dataNode->properties.find("tiling") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["tiling"]->Value<ZOFNumber>(0);
		SetProperty(handle, "tiling", prop->value);
	}

	if (dataNode->properties.find("emission") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["emission"]->Value<ZOFNumber>(0);
		SetProperty(handle, "emission", prop->value);
	}

	if (dataNode->properties.find("ambient") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["ambient"]->Value<ZOFNumber>(0);
		SetProperty(handle, "ambient", prop->value);
	}

	if (dataNode->properties.find("diffuse") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["diffuse"]->Value<ZOFNumber>(0);
		SetProperty(handle, "diffuse", prop->value);
	}

	if (dataNode->properties.find("specular") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["specular"]->Value<ZOFNumber>(0);
		SetProperty(handle, "specular", prop->value);
	}

	if (dataNode->properties.find("shininess") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["shininess"]->Value<ZOFNumber>(0);
		SetProperty(handle, "shininess", prop->value);
	}

	if (dataNode->properties.find("metallic") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["metallic"]->Value<ZOFNumber>(0);
		SetProperty(handle, "metallic", prop->value);
	}

	if (dataNode->properties.find("roughness") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["roughness"]->Value<ZOFNumber>(0);
		SetProperty(handle, "roughness", prop->value);
	}

	if (dataNode->properties.find("ao") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFNumber> prop = dataNode->properties["ao"]->Value<ZOFNumber>(0);
		SetProperty(handle, "ao", prop->value);
	}

    if (dataNode->properties.find("shader") != dataNode->properties.end())
    {
        std::shared_ptr<ZOFHandle> shaderProp = dataNode->properties["shader"]->Value<ZOFHandle>(0);
        SetShader(handle, ZHShader(shaderProp->value));
    }

	if (dataNode->properties.find("height") != dataNode->properties.end())
	{
		SetProperty(handle, "hasDisplacement", true);
	}

	if (dataNode->properties.find("name") != dataNode->properties.end())
	{
        std::shared_ptr<ZOFString> prop = dataNode->properties["name"]->Value<ZOFString>(0);
		material->name = prop->value;
	}

	Track(handle);
}

ZHMaterial ZMaterialManager::Create(const ZMaterialProperties& materialProperties, const ZHShader& shader)
{
	ZHMaterial handle;
	ZMaterialBase* material = resourcePool_.New(handle);

    material->properties = materialProperties;

	UpdateUniformMaterial(handle);

	ZRenderStateGroupWriter writer;
	writer.Begin();
	writer.SetBlending(ZBlendMode::Transluscent);
	writer.BindUniformBuffer(material->uniformBuffer);
    material->renderState = writer.End();

	SetShader(handle, shader);

	ZServices::EventAgent()->Queue(std::make_shared<ZMaterialReadyEvent>(handle));

	Track(handle);

    return handle;
}

ZHMaterial ZMaterialManager::Create(const ZTextureMap& textures, const ZHShader& shader)
{
	ZHMaterial handle;
	ZMaterialBase* material = resourcePool_.New(handle);

	UpdateUniformMaterial(handle);

	ZRenderStateGroupWriter writer;
	writer.Begin();
	writer.SetBlending(ZBlendMode::Transluscent);
	writer.BindUniformBuffer(material->uniformBuffer);
	material->renderState = writer.End();

	for (auto const& [key, val] : textures)
		AddTexture(handle, key, val);

	SetShader(handle, shader);

	ZServices::EventAgent()->Queue(std::make_shared<ZMaterialReadyEvent>(handle));

	Track(handle);

	return handle;
}

void ZMaterialManager::UpdateUniformMaterial(const ZHMaterial& handle)
{
	assert(!handle.IsNull() && "Cannot set property with a null material handle!");
	ZMaterialBase* material = resourcePool_.Get(handle);

    if (!material->uniformBuffer)
        material->uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::Material, sizeof(ZMaterialUniforms));

    bool textured = !material->textures.empty();
    material->uniformBuffer->Update(offsetof(ZMaterialUniforms, material), sizeof(material->properties), &material->properties);
    material->uniformBuffer->Update(offsetof(ZMaterialUniforms, isTextured), sizeof(textured), &textured);
    material->uniformBuffer->Update(offsetof(ZMaterialUniforms, hasDisplacement), sizeof(material->hasDisplacement), &material->hasDisplacement);
}
