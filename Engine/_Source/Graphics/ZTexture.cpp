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
#include "ZResourceExtraData.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZImageImporter.hpp"

std::map<std::string, ZTextureWrapping> ZGLTexture::pendingTextureWrappings_;

ZTexture::ZTexture()
    : id(0), name(std::string()), type(std::string()), path(std::string()), wrapping(ZTextureWrapping::Repeat)
{
}

void ZTexture::Initialize()
{
    ZServices::EventAgent()->Subscribe(this, &ZTexture::HandleTextureLoaded);
}

void ZTexture::Load(const std::string& filename, const std::string& directory, ZTextureWrapping wrapping, bool hdr, bool flip)
{
    std::string filePath = (!directory.empty() ? directory + '/' : "") + filename;
    std::shared_ptr<ZResourceHandle> handle = ZImageImporter::LoadImage(filePath, hdr, flip);
    Load(handle, wrapping, hdr, flip);
}

void ZTexture::LoadAsync(const std::string& filename, const std::string& directory, ZTextureWrapping wrapping, bool hdr, bool flip, bool equirect)
{
    path = (!directory.empty() ? directory + '/' : "") + filename;
    ZResourceType type = ZResourceType::Other;
    if (hdr && equirect) type = ZResourceType::HDREquirectangularMap;
    else if (hdr) type = ZResourceType::HDRTexture;
    else type = ZResourceType::Texture;

    pendingTextureWrappings_[path] = wrapping;

    ZResource textureResource(path, type);
    ZServices::ResourceCache()->RequestHandle(textureResource);
}

void ZTexture::CreateAsync(std::shared_ptr<ZOFTree> data, ZTextureIDMap& outPendingTextures)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0)
        {
            std::string path, type;
            ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (textureNode->properties.find("type") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> typeStr = textureNode->properties["type"]->Value<ZOFString>(0);
                type = typeStr->value;
            }

            if (textureNode->properties.find("wrapping") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> wrappingStr = textureNode->properties["wrapping"]->Value<ZOFString>(0);
                wrapping = wrappingStr->value == "Repeat" ? ZTextureWrapping::Repeat : ZTextureWrapping::EdgeClamp;
            }

            if (!path.empty())
            {
                ZTexture::ptr texture = ZTexture::CreateDefault();
                texture->path = path;
                texture->type = type;
                texture->name = it->first;
                texture->wrapping = wrapping;
                outPendingTextures[texture] = path;
            }
        }
    }

    for (auto it = outPendingTextures.begin(); it != outPendingTextures.end(); it++)
    {
        it->first->LoadAsync(it->second, "", it->first->wrapping);
        it->first->Initialize();
    }
}

void ZTexture::Create(std::shared_ptr<ZOFTree> data, ZTextureMap& outTextureMap)
{
    ZTextureMap textures;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0)
        {
            std::string path, type;
            ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (textureNode->properties.find("type") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> typeStr = textureNode->properties["type"]->Value<ZOFString>(0);
                type = typeStr->value;
            }

            if (textureNode->properties.find("wrapping") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> wrappingStr = textureNode->properties["wrapping"]->Value<ZOFString>(0);
                wrapping = wrappingStr->value == "Repeat" ? ZTextureWrapping::Repeat : ZTextureWrapping::EdgeClamp;
            }

            if (!path.empty())
            {
                ZTexture::ptr texture = Create(path, "", wrapping);
                texture->name = it->first;
                texture->type = type;
                textures[it->first] = texture;
            }
        }
    }
    outTextureMap = textures;
}


ZTexture::ptr ZTexture::Create(const std::string& filename, const std::string& directory, ZTextureWrapping wrapping, bool hdr, bool flip)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->Load(filename, directory, wrapping, hdr, flip);
    return texture;
}

ZTexture::ptr ZTexture::Create(std::shared_ptr<ZResourceHandle> handle, ZTextureWrapping wrapping, bool hdr, bool flip)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->Load(handle, wrapping, hdr, flip);
    return texture;
}

ZTexture::ptr ZTexture::CreateEmptyLUT()
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadEmptyLUT();
    return texture;
}

ZTexture::ptr ZTexture::CreateColor(const glm::vec2& size, bool multisample)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->LoadColor(size, multisample);
    texture->Initialize();
    return texture;
}

ZTexture::ptr ZTexture::CreateDepth(const glm::vec2& size)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->LoadDepth(size);
    texture->Initialize();
    return texture;
}


ZTexture::ptr ZTexture::CreateDefault()
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->LoadDefault();
    return texture;
}

ZTexture::ptr ZTexture::CreateCubeMap(const std::vector<std::string>& faces)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadCubeMap(faces);
    return texture;
}

ZTexture::ptr ZTexture::CreateCubeMap(const ZTexture::ptr& hdrTexture, std::shared_ptr<ZFramebuffer>& bufferData)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadCubeMap(hdrTexture, bufferData);
    return texture;
}

ZTexture::ptr ZTexture::CreateEmptyCubeMap(ZCubemapTextureType type)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadEmptyCubeMap(type);
    return texture;
}

ZTexture::ptr ZTexture::CreateAsync(const std::string& filename, const std::string& directory, ZTextureWrapping wrapping, bool hdr, bool flip, bool equirect)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadAsync(filename, directory, wrapping, hdr, flip, equirect);
    return texture;
}

ZTexture::ptr ZTexture::CreateHDRIAsync(const std::string& hdriPath)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadHDRIAsync(hdriPath);
    return texture;
}

ZTexture::ptr ZTexture::CreateHDRI(const std::string& hdriPath, std::shared_ptr<ZFramebuffer>& bufferData)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadHDRI(hdriPath, bufferData);
    return texture;
}

ZTexture::ptr ZTexture::CreateIrradianceMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadIrradianceMap(cubemapBufferData, cubemapTexture);
    return texture;
}

ZTexture::ptr ZTexture::CreatePrefilterMap(const std::shared_ptr<ZFramebuffer>& cubemapBufferData, const ZTexture::ptr& cubemapTexture)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadPrefilterCubeMap(cubemapBufferData, cubemapTexture);
    return texture;
}

ZTexture::ptr ZTexture::CreateBRDFLUT(const std::shared_ptr<ZFramebuffer>& cubemapBufferData)
{
    // TODO: Switch on contant, variable or define to choose implementation
    ZTexture::ptr texture = std::make_shared<ZGLTexture>();
    texture->Initialize();
    texture->LoadBRDFLUT(cubemapBufferData);
    return texture;
}

void ZTexture::HandleTextureLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    if (!event->Handle()) return;

    ZResource resource = event->Handle()->Resource();
    if (resource.type != ZResourceType::Texture &&
        resource.type != ZResourceType::HDRTexture &&
        resource.type != ZResourceType::HDREquirectangularMap)
        return;
    if (resource.name != path)
        return;

    ZServices::EventAgent()->Unsubscribe(this, &ZTexture::HandleTextureLoaded);

    std::shared_ptr<ZTextureResourceExtraData> textureData = std::static_pointer_cast<ZTextureResourceExtraData>(event->Handle()->ExtraData());

    std::shared_ptr<ZTextureReadyEvent> textureReadyEvent;
    if (resource.type == ZResourceType::HDREquirectangularMap)
    {
        ZTexture::ptr texture = Create(event->Handle(), pendingTextureWrappings_[event->Handle()->Resource().name], textureData->IsHDR(), textureData->IsFlipped());
        pendingTextureWrappings_.erase(event->Handle()->Resource().name);
        ZFramebuffer::ptr bufferData;
        LoadCubeMap(texture, bufferData);
        textureReadyEvent = std::make_shared<ZTextureReadyEvent>(shared_from_this(), bufferData);
    }
    else
    {
        Load(event->Handle(), pendingTextureWrappings_[event->Handle()->Resource().name], textureData->IsHDR(), textureData->IsFlipped());
        pendingTextureWrappings_.erase(event->Handle()->Resource().name);
        textureReadyEvent = std::make_shared<ZTextureReadyEvent>(shared_from_this());
    }
    ZServices::EventAgent()->Queue(textureReadyEvent);
}