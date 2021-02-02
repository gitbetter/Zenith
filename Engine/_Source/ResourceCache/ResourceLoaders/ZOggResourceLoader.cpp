/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZOggResourceLoader.cpp

    Created by Adrian Sanchez on 11/04/2019.
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

#include "ZOggResourceLoader.hpp"
#include "ZResourceExtraData.hpp"
#include "ZServices.hpp"
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

struct OggMemoryFile
{
    unsigned char* data;
    size_t dataSize;
    size_t dataRead;

    OggMemoryFile() : data(nullptr), dataSize(0), dataRead(0) {}
};

size_t VorbisRead(void* data_ptr, size_t byteSize, size_t sizeToRead, void* data_src)
{
    OggMemoryFile* vorbisData = static_cast<OggMemoryFile*>(data_src);
    if (!vorbisData) return -1;

    size_t actualSizeRead, spaceToEOF = vorbisData->dataSize - vorbisData->dataRead;
    if ((sizeToRead * byteSize) < spaceToEOF)
    {
        actualSizeRead = (sizeToRead * byteSize);
    }
    else
    {
        actualSizeRead = spaceToEOF;
    }

    if (actualSizeRead)
    {
        memcpy(data_ptr, (char*) vorbisData->data + vorbisData->dataRead, actualSizeRead);
        vorbisData->dataRead += actualSizeRead;
    }

    return actualSizeRead;
}

int VorbisSeek(void* data_src, long long offset, int origin)
{
    OggMemoryFile* vorbisData = static_cast<OggMemoryFile*>(data_src);
    if (!vorbisData) return -1;

    switch (origin)
    {
    case SEEK_SET:
    {
        long long actualOffset;
        actualOffset = (vorbisData->dataSize >= offset) ? offset : vorbisData->dataSize;
        vorbisData->dataRead = static_cast<size_t>(actualOffset);
        break;
    }
    case SEEK_CUR:
    {
        size_t spaceToEOF = vorbisData->dataSize - vorbisData->dataRead;
        long long actualOffset;
        actualOffset = (offset < spaceToEOF) ? offset : spaceToEOF;

        vorbisData->dataRead += static_cast<long>(actualOffset);
        break;
    }
    case SEEK_END:
        vorbisData->dataRead = vorbisData->dataSize + 1;
        break;
    default:
        LOG("Bad parameter for 'origin', requires same as fseek.", ZSeverity::Error);
        break;
    };

    return 0;
}

int VorbisClose(void* src)
{
    return 0;
}

long VorbisTell(void* data_src)
{
    OggMemoryFile* vorbisData = static_cast<OggMemoryFile*>(data_src);
    if (!vorbisData) return -1L;

    return static_cast<long>(vorbisData->dataRead);
}

unsigned int ZOggResourceLoader::LoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
    OggVorbis_File vf;

    ov_callbacks oggCallbacks;

    OggMemoryFile* vorbisMemoryFile = new OggMemoryFile;
    vorbisMemoryFile->dataRead = 0;
    vorbisMemoryFile->dataSize = rawSize;
    vorbisMemoryFile->data = (unsigned char*) rawBuffer;

    oggCallbacks.read_func = VorbisRead;
    oggCallbacks.close_func = VorbisClose;
    oggCallbacks.seek_func = VorbisSeek;
    oggCallbacks.tell_func = VorbisTell;

    int ov_ret = ov_open_callbacks(vorbisMemoryFile, &vf, NULL, 0, oggCallbacks);
    assert(ov_ret >= 0);

    vorbis_info* vi = ov_info(&vf, -1);

    unsigned long bytes = (unsigned long) ov_pcm_total(&vf, -1);
    bytes *= 2 * vi->channels;

    ov_clear(&vf);
    delete vorbisMemoryFile;

    return bytes;
}

bool ZOggResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ZResourceHandle> handle)
{
    std::shared_ptr<ZSoundResourceExtraData> extra = std::make_shared<ZSoundResourceExtraData>();
    extra->soundType_ = ZSoundType::Ogg;
    handle->SetExtra(extra);
    if (!ParseOgg(rawBuffer, rawSize, handle)) return false;
    return true;
}

bool ZOggResourceLoader::ParseOgg(char* oggStream, unsigned int length, std::shared_ptr<ZResourceHandle> handle)
{
    std::shared_ptr<ZSoundResourceExtraData> extra = std::static_pointer_cast<ZSoundResourceExtraData>(handle->ExtraData());

    OggVorbis_File vf;

    ov_callbacks oggCallbacks;

    OggMemoryFile* vorbisMemoryFile = new OggMemoryFile;
    vorbisMemoryFile->dataRead = 0;
    vorbisMemoryFile->dataSize = length;
    vorbisMemoryFile->data = (unsigned char*) oggStream;

    oggCallbacks.read_func = VorbisRead;
    oggCallbacks.close_func = VorbisClose;
    oggCallbacks.seek_func = VorbisSeek;
    oggCallbacks.tell_func = VorbisTell;

    int ov_ret = ov_open_callbacks(vorbisMemoryFile, &vf, NULL, 0, oggCallbacks);
    assert(ov_ret >= 0);

    vorbis_info* vi = ov_info(&vf, -1);

    memset(&extra->wavFormatDesc_, 0, sizeof(ZWavFormatDesc));
    extra->wavFormatDesc_.cbSize = sizeof(extra->wavFormatDesc_);
    extra->wavFormatDesc_.channels = vi->channels;
    extra->wavFormatDesc_.bitsPerSample = 16;
    extra->wavFormatDesc_.samplesPerSec = vi->rate;
    extra->wavFormatDesc_.avgBytesPerSec = extra->wavFormatDesc_.samplesPerSec * extra->wavFormatDesc_.channels * 2;
    extra->wavFormatDesc_.blockAlign = extra->wavFormatDesc_.channels * 2;
    extra->wavFormatDesc_.formatTag = 1;

    unsigned long size = 4096 * 16, pos = 0;
    int sec = 0, ret = 1;

    unsigned long bytes = (unsigned long) ov_pcm_total(&vf, -1);
    bytes *= 2 * vi->channels;

    if (handle->Size() != bytes)
    {
        LOG("The OGG file size and the memory buffer size are not the same", ZSeverity::Error);
        ov_clear(&vf);
        delete vorbisMemoryFile;
        return false;
    }

    while (ret && pos < bytes)
    {
        ret = ov_read(&vf, (char*) handle->FluidBuffer() + pos, size, 0, 2, 1, &sec);
        pos += ret;
        if (bytes - pos < size) size = bytes - pos;
    }

    extra->lengthMilli_ = (int) (1000.f * ov_time_total(&vf, -1));

    ov_clear(&vf);
    delete vorbisMemoryFile;

    return true;
}
