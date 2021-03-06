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
#include "ZResourceExtraData.hpp"

std::mutex ZImageImporter::importerMutex_;

std::shared_ptr<ZResourceHandle> ZImageImporter::LoadImage(const std::string& path, bool hdr, bool flipped)
{
    ZResource resource(path, hdr ? ZResourceType::HDRTexture : ZResourceType::Texture);
    std::shared_ptr<ZResourceHandle> handle = ZServices::ResourceCache()->GetHandle(&resource);
    return LoadImage(handle, hdr, flipped);
}

std::shared_ptr<ZResourceHandle> ZImageImporter::LoadImage(const std::shared_ptr<ZResourceHandle>& handle, bool hdr, bool flipped)
{
    if (!handle) return nullptr;

    std::shared_ptr<ZTextureResourceExtraData> extraData = std::make_shared<ZTextureResourceExtraData>();
    extraData->hdr_ = hdr;
    extraData->flipped_ = flipped;

    importerMutex_.lock();
    stbi_set_flip_vertically_on_load(flipped);

    if (hdr)
    {
        extraData->fData_ = stbi_loadf_from_memory((const stbi_uc*) handle->Buffer(), handle->Size(), &extraData->width_, &extraData->height_, &extraData->channels_, 0);
    }
    else
    {
        extraData->ucData_ = stbi_load_from_memory((const stbi_uc*) handle->Buffer(), handle->Size(), &extraData->width_, &extraData->height_, &extraData->channels_, 4);
    }
    importerMutex_.unlock();

    handle->SetExtra(extraData);

    return handle;
}

void ZImageImporter::FreeImageData(void* data)
{
    stbi_image_free(data);
}
