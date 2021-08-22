/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourceLoadTask.cpp

    Created by Adrian Sanchez on 12/05/2019.
    Copyright � 2019 Pervasive Sense. All rights reserved.

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
#include "ZResourceLoadTask.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZOFParser.hpp"
#include "ZModelImporter.hpp"
#include "ZImageImporter.hpp"

void ZResourceLoadTask::Run()
{
    ZServices::ResourceCache()->GetData(resource_.get());

    if (resource_->size == 0)
    {
        LOG("Could not find resource at path " + resource_->path, ZSeverity::Error);
        return;
    }

    switch (resource_->type)
    {
    case ZResourceType::VertexShader:
    case ZResourceType::PixelShader:
    case ZResourceType::TesselationShader:
    case ZResourceType::GeometryShader:
    {
		std::shared_ptr<ZShaderResourceData> shaderData = std::static_pointer_cast<ZShaderResourceData>(resource_);
        shaderData->code = std::string(static_cast<char*>(resource_->buffer));
    }
    case ZResourceType::ZOF:
    {
        ZOFParser parser;
        std::shared_ptr<ZZofResourceData> zofData = std::static_pointer_cast<ZZofResourceData>(resource_);
        zofData->objectTree = parser.Parse(std::string((const char*)resource_->buffer));
        break;
    }
    case ZResourceType::HDREquirectangularMap:
    case ZResourceType::HDRTexture:
    case ZResourceType::Texture:
    {
        std::shared_ptr<ZTextureResourceData> textureData = std::static_pointer_cast<ZTextureResourceData>(resource_);
        ZImageImporter::LoadImage(textureData.get(),
            resource_->type == ZResourceType::HDRTexture || resource_->type == ZResourceType::HDREquirectangularMap,
            resource_->type == ZResourceType::HDRTexture || resource_->type == ZResourceType::HDREquirectangularMap);
        break;
    }
    case ZResourceType::Model:
    {
		ZModelImporter importer;
		std::string modelDirectory = resource_->path.substr(0, resource_->path.find_last_of("/\\"));
        std::shared_ptr<ZModelResourceData> modelData = std::static_pointer_cast<ZModelResourceData>(resource_);
        modelData->meshMap = importer.LoadModel(modelData.get(), modelData->boneMap, modelData->boneList, modelData->animationMap, modelData->skeleton, modelDirectory);
        break;
    }
    default: break;
    }

    std::shared_ptr<ZResourceLoadedEvent> loadedEvent = std::make_shared<ZResourceLoadedEvent>(resource_);
    ZServices::EventAgent()->Queue(loadedEvent);

    // Makes sure the process is marked as completed, removed from the process runner, destroyed and the thread joins
    Finish();
}