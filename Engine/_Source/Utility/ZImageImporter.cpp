/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZImageImporter.cpp

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
#include "ZImageImporter.hpp"
#include "ZResourceData.hpp"

std::mutex ZImageImporter::importerMutex_;

ZTextureResourceData::ptr ZImageImporter::LoadImage(const std::string& path, bool hdr, bool flipped, ZTextureWrapping wrapping, const std::string& type)
{
    ZTextureResourceData::ptr resource = std::make_shared<ZTextureResourceData>(path, wrapping, type, ZHTexture(), hdr ? ZResourceType::HDRTexture : ZResourceType::Texture);
    ZServices::ResourceImporter()->GetData(resource.get());
    LoadImage(resource.get());
    return resource;
}

void ZImageImporter::LoadImage(ZTextureResourceData* resource)
{
    if (resource == nullptr) return;

    importerMutex_.lock();
    stbi_set_flip_vertically_on_load(resource->flipped);

    if (resource->hdr)
    {
        resource->data = stbi_loadf_from_memory((const stbi_uc*) resource->buffer, resource->size, &resource->width, &resource->height, &resource->channels, 0);
    }
    else
    {
        resource->data = stbi_load_from_memory((const stbi_uc*) resource->buffer, resource->size, &resource->width, &resource->height, &resource->channels, 4);
    }
    importerMutex_.unlock();
}

void ZImageImporter::FreeImageData(void* data)
{
    stbi_image_free(data);
}
