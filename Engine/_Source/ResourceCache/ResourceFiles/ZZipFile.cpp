/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZZipFile.cpp

    Created by Adrian Sanchez on 09/03/2019.
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

#include "ZZipFile.hpp"

bool ZZipFile::Open()
{
    zipFile_ = zip_open(fileName_.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');

    if (!zipFile_)
    {
        zenith::Log("Failed to open zip file " + fileName_, ZSeverity::Error);
        return false;
    }

    return true;
}

unsigned int ZZipFile::RawResourceSize(const ZResource& resource)
{
    zip_entry_open(zipFile_, resource.name.c_str());
    unsigned int size = zip_entry_size(zipFile_);
    zip_entry_close(zipFile_);
    return size;
}

unsigned int ZZipFile::RawResource(const ZResource& resource, char* buffer)
{
    if (zipFile_)
    {
        zip_entry_open(zipFile_, resource.name.c_str());
        size_t bytes = zip_entry_size(zipFile_);
        zip_entry_noallocread(zipFile_, (void*) buffer, bytes);
        zip_entry_close(zipFile_);
        return (unsigned int) bytes;
    }
    return 0;
}

unsigned int ZZipFile::ResourceCount() const
{
    return zipFile_ ? zip_total_entries(zipFile_) : 0;
}

std::string ZZipFile::ResourceName(unsigned int num) const
{
    zip_entry_openbyindex(zipFile_, num);
    std::string name = std::string(zip_entry_name(zipFile_));
    zip_entry_close(zipFile_);
    return name;
}

void ZZipFile::PrintResources() const
{
    if (zipFile_)
    {
        int i, n = zip_total_entries(zipFile_);
        for (i = 0; i < n; ++i)
        {
            zip_entry_openbyindex(zipFile_, i);
            {
                const char* name = zip_entry_name(zipFile_);
    //            int isdir = zip_entry_isdir(zipFile_);
    //            unsigned long long size = zip_entry_size(zipFile_);
    //            unsigned int crc32 = zip_entry_crc32(zipFile_);
                zenith::Log(name, ZSeverity::Error);
            }
            zip_entry_close(zipFile_);
        }
    }
}

void ZZipFile::Close()
{
    if (zipFile_) zip_close(zipFile_);
    zipFile_ = nullptr;
}
