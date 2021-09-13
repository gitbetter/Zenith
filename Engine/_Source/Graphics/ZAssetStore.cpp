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

    ZServices::EventAgent()->Subscribe(this, &ZAssetStore::HandleModelReady);
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

void ZAssetStore::Load(std::shared_ptr<ZOFNode> root)
{
    ZServices::FontManager()->Deserialize(root, fonts);
    ZServices::TextureManager()->Deserialize(root, textures);
    ZServices::ShaderManager()->Deserialize(root, shaders);
    ZModel::Deserialize(root, models);
    ZServices::MaterialManager()->Deserialize(root);

    for (ZModelMap::iterator it = models.begin(); it != models.end(); it++)
    {
        AddModel(it->first, it->second);
    }
}

void ZAssetStore::LoadAsync(std::shared_ptr<ZOFNode> root)
{
    ZModelMap loadedModels;

    ZServices::FontManager()->DeserializeAsync(root);
    ZServices::TextureManager()->DeserializeAsync(root);
    ZServices::ShaderManager()->DeserializeAsync(root);
    ZModel::CreateAsync(root, pendingModels_, loadedModels);
    ZServices::MaterialManager()->DeserializeAsync(root);

    for (ZModelMap::iterator it = loadedModels.begin(); it != loadedModels.end(); it++)
    {
        AddModel(it->first, it->second);
    }
}

void ZAssetStore::AddModel(const std::string& id, std::shared_ptr<ZModel> model)
{
    loadedModels_[id] = model;
}

bool ZAssetStore::HasModel(const std::string& id) const
{
    return loadedModels_.find(id) != loadedModels_.end();
}

std::shared_ptr<ZModel> ZAssetStore::GetModel(const std::string& id)
{
    if (loadedModels_.find(id) != loadedModels_.end()) {
        return loadedModels_[id];
    }
    return nullptr;
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

void ZAssetStore::CleanUp()
{
    ZServices::EventAgent()->Unsubscribe(this, &ZAssetStore::HandleModelReady);
}
