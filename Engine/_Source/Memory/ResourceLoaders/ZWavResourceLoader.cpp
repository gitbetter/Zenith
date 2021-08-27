/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZWavResourceLoader.cpp

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

#include "ZWavResourceLoader.hpp"
#include "ZServices.hpp"

#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
	((unsigned long)(unsigned char)(ch0) | ((unsigned long)(unsigned char)(ch1) << 8) |\
   ((unsigned long)(unsigned char)(ch2) << 16) | ((unsigned long)(unsigned char)(ch3) << 24))

unsigned int ZWavResourceLoader::LoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
    unsigned long file = 0, fileEnd = 0, length = 0, type = 0;
    unsigned long pos = 0;

    type = *((unsigned long*) (rawBuffer + pos)); pos += sizeof(unsigned long);
    if (type != MAKEFOURCC('R', 'I', 'F', 'F'))
        return false;

    length = *((unsigned long*) (rawBuffer + pos)); pos += sizeof(unsigned long);
    type = *((unsigned long*) (rawBuffer + pos)); pos += sizeof(unsigned long);
    if (type != MAKEFOURCC('W', 'A', 'V', 'E'))
        return false;

    fileEnd = length - 4;

    while (file < fileEnd)
    {
        type = *((unsigned long*) (rawBuffer + pos)); pos += sizeof(unsigned long);
        file += sizeof(unsigned long);

        length = *((unsigned long*) (rawBuffer + pos)); pos += sizeof(unsigned long);
        file += sizeof(unsigned long);

        switch (type)
        {
        case MAKEFOURCC('f', 'a', 'c', 't'):
        {
            LOG("Compressed .wav files are not yet supported", ZSeverity::Warning);
            break;
        }
        case MAKEFOURCC('f', 'm', 't', ' '):
        {
            pos += length;
            break;
        }
        case MAKEFOURCC('L', 'I', 'S', 'T'):
        case MAKEFOURCC('I', 'N', 'F', 'O'):
        {
            pos += length;
            break;
        }
        case MAKEFOURCC('d', 'a', 't', 'a'):
        {
            return length;
        }
        }

        file += length;

        if (length & 1)
        {
            ++pos; ++file;
        }
    }

    return 0;
}

bool ZWavResourceLoader::Load(char* rawBuffer, unsigned int rawSize, ZAudioResourceData* resource)
{
    resource->soundType = ZSoundType::Wav;
    if (!ParseWav(rawBuffer, rawSize, resource)) return false;
    return true;
}


bool ZWavResourceLoader::ParseWav(char* wavStream, unsigned int bufferLength, ZAudioResourceData* resource)
{
    unsigned long file = 0, fileEnd = 0, length = 0, type = 0;
    unsigned long pos = 0;

    type = *((unsigned long*) (wavStream + pos)); pos += sizeof(unsigned long);
    if (type != MAKEFOURCC('R', 'I', 'F', 'F'))
        return false;

    length = *((unsigned long*) (wavStream + pos)); pos += sizeof(unsigned long);
    type = *((unsigned long*) (wavStream + pos)); pos += sizeof(unsigned long);
    if (type != MAKEFOURCC('W', 'A', 'V', 'E'))
        return false;

    fileEnd = length - 4;

    memset(&resource->wavFormatDesc, 0, sizeof(ZWavFormatDesc));

    bool copiedBuffer = false;
    while (file < fileEnd)
    {
        type = *((unsigned long*) (wavStream + pos)); pos += sizeof(unsigned long);
        file += sizeof(unsigned long);

        length = *((unsigned long*) (wavStream + pos)); pos += sizeof(unsigned long);
        file += sizeof(unsigned long);

        switch (type)
        {
        case MAKEFOURCC('f', 'a', 'c', 't'):
        {
            LOG("Compressed .wav files are not yet supported", ZSeverity::Warning);
            break;
        }
        case MAKEFOURCC('f', 'm', 't', ' '):
        {
            memcpy(&resource->wavFormatDesc, wavStream + pos, length); pos += length;
            resource->wavFormatDesc.cbSize = (unsigned short) length;
            break;
        }
        case MAKEFOURCC('L', 'I', 'S', 'T'):
        case MAKEFOURCC('I', 'N', 'F', 'O'):
        {
            pos += length; // Skip the metadata
            break;
        }
        case MAKEFOURCC('d', 'a', 't', 'a'):
        {
            copiedBuffer = true;
            if (length != resource->size)
            {
                LOG("Resource size and buffer size do not match", ZSeverity::Error);
                return 0;
            }
            memcpy(resource->buffer, wavStream + pos, length); pos += length;
            break;
        }
        }

        file += length;

        if (copiedBuffer)
        {
            resource->lengthMilli = (resource->size * 1000) / resource->wavFormatDesc.avgBytesPerSec;
            return true;
        }

        if (length & 1)
        {
            ++pos; ++file;
        }
    }

    return false;
}
