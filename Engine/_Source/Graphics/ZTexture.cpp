/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZTexture.cpp

    Created by Adrian Sanchez on 24/01/2021.
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

#include "ZGLTexture.hpp"
#include "ZServices.hpp"
#include "ZFramebuffer.hpp"
#include "ZOFTree.hpp"
#include "ZResourceData.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZImageImporter.hpp"

ZIDSequence ZTexture::idGenerator_;

ZTexture::ZTexture()
{
	name = "Texture_" + idGenerator_.Next();
}

unsigned int ZTextureManager::PlatformHandle(const ZHTexture& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	return texture->id;
}

std::string ZTextureManager::Name(const ZHTexture& handle)
{
    assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
    ZTexture* texture = resourcePool_.Get(handle);
	return texture->name;
}

std::string ZTextureManager::Type(const ZHTexture& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	return texture->type;
}

std::string ZTextureManager::Path(const ZHTexture& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	return texture->path;
}

bool ZTextureManager::IsMultisampled(const ZHTexture& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	return texture->multisampled;
}

ZTextureWrapping ZTextureManager::Wrapping(const ZHTexture& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	return texture->wrapping;
}

void ZTextureManager::SetPlatformHandle(const ZHTexture& handle, unsigned int pHandle)
{
	assert(!handle.IsNull() && "Cannot set property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	texture->id = pHandle;
}

void ZTextureManager::SetName(const ZHTexture& handle, const std::string& name)
{
	assert(!handle.IsNull() && "Cannot set property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	texture->name = name;
}

void ZTextureManager::SetType(const ZHTexture& handle, const std::string& type)
{
	assert(!handle.IsNull() && "Cannot set property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	texture->type = type;
}

void ZTextureManager::SetPath(const ZHTexture& handle, const std::string& path)
{
	assert(!handle.IsNull() && "Cannot set property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	texture->path = path;
}

void ZTextureManager::SetIsMultisampled(const ZHTexture& handle, bool multisampled)
{
	assert(!handle.IsNull() && "Cannot set property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	texture->multisampled = multisampled;
}

void ZTextureManager::SetWrapping(const ZHTexture& handle, ZTextureWrapping wrapping)
{
	assert(!handle.IsNull() && "Cannot set property with a null texture handle!");
	ZTexture* texture = resourcePool_.Get(handle);
	texture->wrapping = wrapping;
}

void ZTextureManager::Initialize()
{
    ZServices::EventAgent()->Subscribe(this, &ZTextureManager::HandleTextureLoaded);
}

void ZTextureManager::CleanUp()
{
    ZServices::EventAgent()->Unsubscribe(this, &ZTextureManager::HandleTextureLoaded);
}

ZHTexture ZTextureManager::Deserialize(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
	if (dataNode->type != ZOFObjectType::Texture)
	{
		return ZHTexture();
	}

	ZHTexture restoreHandle(dataHandle.value);

	std::string path, type;
	ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp;
	if (dataNode->properties.find("path") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> pathStr = dataNode->properties["path"]->Value<ZOFString>(0);
		path = pathStr->value;
	}

	if (dataNode->properties.find("type") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> typeStr = dataNode->properties["type"]->Value<ZOFString>(0);
		type = typeStr->value;
	}

	if (dataNode->properties.find("wrapping") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> wrappingStr = dataNode->properties["wrapping"]->Value<ZOFString>(0);
		wrapping = wrappingStr->value == "Repeat" ? ZTextureWrapping::Repeat : ZTextureWrapping::EdgeClamp;
	}

	if (!path.empty())
	{
		return Create(path, type, wrapping, false, true, restoreHandle);
	}

	return ZHTexture();
}

void ZTextureManager::DeserializeAsync(const ZOFHandle& dataHandle, std::shared_ptr<ZOFObjectNode> dataNode)
{
    if (dataNode->type != ZOFObjectType::Texture)
    {
        return;
    }

	ZHTexture restoreHandle(dataHandle.value);

	std::string path, type;
	ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp;
	if (dataNode->properties.find("path") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> pathStr = dataNode->properties["path"]->Value<ZOFString>(0);
		path = pathStr->value;
	}

	if (dataNode->properties.find("type") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> typeStr = dataNode->properties["type"]->Value<ZOFString>(0);
		type = typeStr->value;
	}

	if (dataNode->properties.find("wrapping") != dataNode->properties.end())
	{
		std::shared_ptr<ZOFString> wrappingStr = dataNode->properties["wrapping"]->Value<ZOFString>(0);
		wrapping = wrappingStr->value == "Repeat" ? ZTextureWrapping::Repeat : ZTextureWrapping::EdgeClamp;
	}

	if (!path.empty())
	{
		CreateAsync(path, type, wrapping, false, true, false, restoreHandle);
	}
}

ZHTexture ZTextureManager::Create()
{
    // TODO: Switch on constant, variable or define to choose implementation
    ZHTexture handle;
    ZTexture* texture = resourcePool_.New(handle);
    return handle;
}

ZHTexture ZTextureManager::Create(const std::string& path, const std::string& type, ZTextureWrapping wrapping, bool hdr, bool flip, const ZHTexture& restoreHandle)
{
    // TODO: Switch on constant, variable or define to choose implementation
	ZTextureResourceData::ptr resource = ZImageImporter::LoadImage(path, hdr, flip, wrapping, type);
	return Create(resource.get(), type, wrapping, hdr, flip, restoreHandle);
}

void ZTextureManager::CreateAsync(const std::string& path, const std::string& type, ZTextureWrapping wrapping, bool hdr, bool flip, bool equirect, const ZHTexture& restoreHandle)
{
    // TODO: Switch on constant, variable or define to choose implementation
	ZResourceType resourceType = ZResourceType::Other;
	if (hdr && equirect) resourceType = ZResourceType::HDREquirectangularMap;
	else if (hdr) resourceType = ZResourceType::HDRTexture;
	else resourceType = ZResourceType::Texture;

	ZTextureResourceData::ptr resource = std::make_shared<ZTextureResourceData>(path, resourceType, wrapping, type, restoreHandle);
	resource->flipped = flip;
	resource->hdr = hdr;
	ZServices::ResourceImporter()->GetDataAsync(resource);
}

ZIBLTextureData ZTextureManager::CreateIBL(const std::shared_ptr<ZFramebuffer>& bufferData, const ZHTexture& cubemap)
{
    ZIBLTextureData generatedIBLTexture;
    generatedIBLTexture.cubeMap = cubemap;
    generatedIBLTexture.irradiance = CreateIrradianceMap(bufferData, cubemap);
    generatedIBLTexture.prefiltered = CreatePrefilterMap(bufferData, cubemap);
    generatedIBLTexture.brdfLUT = CreateBRDFLUT(bufferData);
    return generatedIBLTexture;
}

ZHTexture ZTextureManager::Default()
{
    static ZHTexture defaultTexture = CreateDefault();
    return defaultTexture;
}

void ZTextureManager::HandleTextureLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (event->Resource() == nullptr)
    {
        return;
    }

    ZResourceData::ptr resource = event->Resource();
    if (resource->type != ZResourceType::Texture &&
        resource->type != ZResourceType::HDRTexture &&
        resource->type != ZResourceType::HDREquirectangularMap)
        return;

    std::shared_ptr<ZTextureResourceData> textureData = std::static_pointer_cast<ZTextureResourceData>(resource);

    if (loadedResources_.find(textureData->path) != loadedResources_.end())
    {
        return;
    }

    std::shared_ptr<ZTextureReadyEvent> textureReadyEvent;
	ZHTexture handle = Create(textureData.get(), textureData->textureType, textureData->wrapping, textureData->hdr, textureData->flipped, textureData->restoreHandle);
    if (resource->type == ZResourceType::HDREquirectangularMap)
    {

        ZFramebuffer::ptr bufferData;
        ZHTexture cubemap = CreateCubeMap(handle, bufferData);
        textureReadyEvent = std::make_shared<ZTextureReadyEvent>(cubemap, bufferData);
    }
    else
    {
        textureReadyEvent = std::make_shared<ZTextureReadyEvent>(handle);
    }
    ZServices::EventAgent()->Queue(textureReadyEvent);
}