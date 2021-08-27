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
#include "ZMaterial.hpp"
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
    ZServices::EventAgent()->Subscribe(this, &ZAssetStore::HandleFontReady);
}

void ZAssetStore::InitializeShaders()
{
    pbrShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/pbr.frag");
	blinnPhongShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/blinnphong.vert", "/Shaders/Pixel/blinnphong.frag");
    debugShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/debug.vert", "/Shaders/Pixel/debug.frag");
    shadowShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/shadow.vert", "/Shaders/Pixel/null.frag");
    depthShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/depth.vert", "/Shaders/Pixel/null.frag");
    postShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/postprocess.vert", "/Shaders/Pixel/postprocess.frag");
    uiShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/ui.vert", "/Shaders/Pixel/ui.frag");
    textShader_ = ZServices::ShaderManager()->Create("/Shaders/Vertex/text.vert", "/Shaders/Pixel/text.frag");
}

void ZAssetStore::InitializeFonts()
{
    auto arialDefault = ZFont::Create("/Fonts/arial/arial.ttf", 64.f);
    AddFont(arialDefault->Name(), arialDefault);
}

void ZAssetStore::Load(std::shared_ptr<ZOFNode> root)
{
    ZShaderMap shaders; ZTextureMap textures; ZModelMap models;
    ZMaterialMap materials; ZFontMap fonts;

    ZFont::Create(root, fonts);
    ZServices::TextureManager()->Create(root, textures);
    ZServices::ShaderManager()->Create(root, shaders);
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

void ZAssetStore::LoadAsync(std::shared_ptr<ZOFNode> root)
{
    ZMaterialMap loadedMaterials; ZModelMap loadedModels;

    ZFont::CreateAsync(root, pendingFonts_);
    ZServices::TextureManager()->CreateAsync(root);
    ZServices::ShaderManager()->CreateAsync(root);
    ZModel::CreateAsync(root, pendingModels_, loadedModels);
    ZMaterial::CreateAsync(root, pendingMaterials_, loadedMaterials);

    for (ZModelMap::iterator it = loadedModels.begin(); it != loadedModels.end(); it++)
    {
        AddModel(it->first, it->second);
    }

    for (ZMaterialMap::iterator it = loadedMaterials.begin(); it != loadedMaterials.end(); it++)
    {
        AddMaterial(it->first, it->second);
    }
}

void ZAssetStore::RegisterFont(const std::string& fontPath, unsigned int fontSize)
{
    ZFont::CreateAsync(fontPath, fontSize);
}

void ZAssetStore::AddFont(const std::string& id, std::shared_ptr<ZFont> font)
{
    loadedFonts_[id] = font;
}

void ZAssetStore::AddShader(const std::string& id, const ZHShader& shader)
{
    if (shader) loadedShaders_[id] = shader;
}

void ZAssetStore::AddTexture(const std::string& id, const ZHTexture& texture)
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

ZHTexture ZAssetStore::GetTexture(const std::string& id)
{
    if (loadedTextures_.find(id) != loadedTextures_.end()) {
        return loadedTextures_[id];
    }
    return ZHTexture();
}

std::shared_ptr<ZModel> ZAssetStore::GetModel(const std::string& id)
{
    if (loadedModels_.find(id) != loadedModels_.end()) {
        return loadedModels_[id];
    }
    return nullptr;
}

ZHShader ZAssetStore::GetShader(const std::string& id)
{
    if (loadedShaders_.find(id) != loadedShaders_.end()) {
        return loadedShaders_[id];
    }
    return ZHShader();
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
	if (event->Shader().IsNull())
	{
		return;
	}

    ZHShader shader = event->Shader();
    AddShader(ZServices::ShaderManager()->Name(shader), shader);
}

void ZAssetStore::HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event)
{
    if (event->Texture().IsNull())
    {
        return;
    }

	ZHTexture texture = event->Texture();
	AddTexture(ZServices::TextureManager()->Name(texture), texture);
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
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleFontReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleShaderReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleTextureReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleModelReady);
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleMaterialReady);
}
