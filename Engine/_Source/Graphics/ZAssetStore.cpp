/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZAssetStore.hpp

    Created by Adrian Sanchez on 31/01/2021.
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

#include "ZAssetStore.hpp"
#include "ZServices.hpp"
#include "ZTexture.hpp"
#include "ZShader.hpp"
#include "ZModel.hpp"
#include "ZFont.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZFontReadyEvent.hpp"
#include "ZMaterialReadyEvent.hpp"

void ZAssetStore::Initialize()
{
    InitializeShaders();
    InitializeFonts();

    ZServices::EventAgent()->Subscribe(this, &ZAssetStore::HandleShaderReady);
    ZServices::EventAgent()->Subscribe(this, &ZAssetStore::HandleTextureReady);
    ZServices::EventAgent()->Subscribe(this, &ZAssetStore::HandleModelReady);
    ZServices::EventAgent()->Subscribe(this, &ZAssetStore::HandleMaterialReady);
}

void ZAssetStore::InitializeShaders()
{
    pbrShader_ = std::unique_ptr<ZShader>(new ZShader("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/pbr.frag"));
    debugShader_ = std::unique_ptr<ZShader>(new ZShader("/Shaders/Vertex/debug.vert", "/Shaders/Pixel/debug.frag"));
    shadowShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/shadow.vert", "/Shaders/Pixel/depth.frag"));
    depthShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/depth.vert", "/Shaders/Pixel/depth.frag"));
    postShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/postprocess.vert", "/Shaders/Pixel/postprocess.frag"));
    uiShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/ui.vert", "/Shaders/Pixel/ui.frag"));
    textShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/text.vert", "/Shaders/Pixel/text.frag"));
    pbrShader_->Initialize();
    debugShader_->Initialize();
    shadowShader_->Initialize();
    depthShader_->Initialize();
    postShader_->Initialize();
    uiShader_->Initialize();
    textShader_->Initialize();
}

void ZAssetStore::InitializeFonts()
{
    auto arialDefault = ZFont::Create("/Fonts/arial/arial.ttf", 64.f);
    AddFont(arialDefault->Name(), arialDefault);
}

void ZAssetStore::Load(std::shared_ptr<ZOFTree> root)
{
    ZShaderMap shaders; ZTextureMap textures; ZModelMap models;
    ZMaterialMap materials; ZFontMap fonts;

    ZFont::Create(root, fonts);
    ZTexture::Create(root, textures);
    ZShader::Create(root, shaders);
    ZModel::Create(root, models);
    ZMaterial::Create(root, materials, textures);

    for (ZFontMap::iterator it = fonts.begin(); it != fonts.end(); it++)
    {
        AddFont(it->first, it->second);
    }

    for (ZTextureMap::iterator it = textures.begin(); it != textures.end(); it++)
    {
        AddTexture(it->first, it->second);
    }

    for (ZShaderMap::iterator it = shaders.begin(); it != shaders.end(); it++)
    {
        AddShader(it->first, it->second);
    }

    for (ZModelMap::iterator it = models.begin(); it != models.end(); it++)
    {
        AddModel(it->first, it->second);
    }

    for (ZMaterialMap::iterator it = materials.begin(); it != materials.end(); it++)
    {
        AddMaterial(it->first, it->second);
    }
}

void ZAssetStore::LoadAsync(std::shared_ptr<ZOFTree> root)
{
    ZFont::CreateAsync(root, pendingFonts_);
    ZTexture::CreateAsync(root, pendingTextures_);
    ZShader::CreateAsync(root, pendingShaders_);
    ZModel::CreateAsync(root, pendingModels_);
    ZMaterial::CreateAsync(root, pendingMaterials_);
}

void ZAssetStore::RegisterFont(const std::string& fontPath, unsigned int fontSize)
{
    ZFont::CreateAsync(fontPath, fontSize);
}

void ZAssetStore::AddFont(const std::string& id, std::shared_ptr<ZFont> font)
{
    loadedFonts_[id] = font;
}

void ZAssetStore::AddShader(const std::string& id, std::shared_ptr<ZShader> shader)
{
    if (shader) loadedShaders_[id] = shader;
}

void ZAssetStore::AddTexture(const std::string& id, const std::shared_ptr<ZTexture>& texture)
{
    loadedTextures_[id] = texture;
}

void ZAssetStore::AddModel(const std::string& id, std::shared_ptr<ZModel> model)
{
    loadedModels_[id] = model;
}

void ZAssetStore::AddMaterial(const std::string& id, std::shared_ptr<ZMaterial> material)
{
    loadedMaterials_[id] = material;
}

bool ZAssetStore::HasFont(const std::string& id) const
{
    return loadedFonts_.find(id) != loadedFonts_.end();
}

bool ZAssetStore::HasTexture(const std::string& id) const
{
    return loadedTextures_.find(id) != loadedTextures_.end();
}

bool ZAssetStore::HasModel(const std::string& id) const
{
    return loadedModels_.find(id) != loadedModels_.end();
}

bool ZAssetStore::HasShader(const std::string& id) const
{
    return loadedShaders_.find(id) != loadedShaders_.end();
}

bool ZAssetStore::HasMaterial(const std::string& id) const
{
    return loadedMaterials_.find(id) != loadedMaterials_.end();
}

ZFont::ptr ZAssetStore::GetFont(const std::string& id)
{
    if (loadedFonts_.find(id) != loadedFonts_.end()) {
        return loadedFonts_[id];
    }
    return nullptr;
}

std::shared_ptr<ZTexture> ZAssetStore::GetTexture(const std::string& id)
{
    if (loadedTextures_.find(id) != loadedTextures_.end()) {
        return loadedTextures_[id];
    }
    return nullptr;
}

std::shared_ptr<ZModel> ZAssetStore::GetModel(const std::string& id)
{
    if (loadedModels_.find(id) != loadedModels_.end()) {
        return loadedModels_[id];
    }
    return nullptr;
}

std::shared_ptr<ZShader> ZAssetStore::GetShader(const std::string& id)
{
    if (loadedShaders_.find(id) != loadedShaders_.end()) {
        return loadedShaders_[id];
    }
    return nullptr;
}

std::shared_ptr<ZMaterial> ZAssetStore::GetMaterial(const std::string& id)
{
    if (loadedMaterials_.find(id) != loadedMaterials_.end()) {
        return loadedMaterials_[id];
    }
    return nullptr;
}

void ZAssetStore::HandleShaderReady(const std::shared_ptr<ZShaderReadyEvent>& event)
{
    if (pendingShaders_.find(event->Shader()) != pendingShaders_.end())
    {
        std::shared_ptr<ZShader> shader = event->Shader();
        AddShader(pendingShaders_[shader], shader);
        pendingShaders_.erase(shader);
    }
}

void ZAssetStore::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    if (pendingTextures_.find(event->Texture()) != pendingTextures_.end())
    {
        ZTexture::ptr texture = event->Texture();
        AddTexture(pendingTextures_[texture], texture);
        pendingTextures_.erase(texture);
    }
}

void ZAssetStore::HandleModelReady(const std::shared_ptr<ZModelReadyEvent>& event)
{
    if (pendingModels_.find(event->Model()) != pendingModels_.end())
    {
        std::shared_ptr<ZModel> model = event->Model();
        AddModel(pendingModels_[model], model);
        pendingModels_.erase(model);
    }
}

void ZAssetStore::HandleFontReady(const std::shared_ptr<ZFontReadyEvent>& event)
{
    if (pendingFonts_.find(event->Font()) != pendingFonts_.end()) {
        std::shared_ptr<ZFont> font = event->Font();
        AddFont(pendingFonts_[font], font);
        pendingFonts_.erase(font);
    }
}

void ZAssetStore::HandleMaterialReady(const std::shared_ptr<ZMaterialReadyEvent>& event)
{
    if (pendingMaterials_.find(event->Material()) != pendingMaterials_.end()) {
        std::shared_ptr<ZMaterial> material = event->Material();
        AddMaterial(pendingMaterials_[material], material);
        pendingMaterials_.erase(material);
    }
}

void ZAssetStore::CleanUp()
{
    if (shadowShader_)
    {
        shadowShader_.reset();
    }
    if (depthShader_)
    {
        depthShader_.reset();
    }
    if (debugShader_)
    {
        debugShader_.reset();
    }
    if (postShader_)
    {
        postShader_.reset();
    }
    if (uiShader_)
    {
        uiShader_.reset();
    }
    if (textShader_)
    {
        textShader_.reset();
    }

    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleShaderReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleTextureReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleModelReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleMaterialReady);
}
