/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZDevResourceFile.cpp

        Created by Adrian Sanchez on 06/04/2019.
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

#include "ZDevResourceFile.hpp"
#include "ZServices.hpp"
#include <cppfs/FileIterator.h>

using namespace cppfs;

bool ZDevResourceFile::Open()
{
    FileHandle handle = fs::open(fileName_);
    if (!handle.exists() || !handle.isDirectory())
    {
        LOG("Development resource file is invalid", ZSeverity::Error);
        return false;
    }

    ReadDirectoryEntries(handle);
    return true;
}

unsigned int ZDevResourceFile::RawResourceSize(const ZResource& resource)
{
    auto it = assetIndices_.find(resource.name);
    if (it == assetIndices_.end())
        return 0;

    return assets_[it->second].size();
}

unsigned int ZDevResourceFile::RawResource(const ZResource& resource, char* buffer)
{
    auto it = assetIndices_.find(resource.name);
    if (it == assetIndices_.end())
        return 0;

    std::unique_ptr<std::istream> in = assets_[it->second].createInputStream(std::ios_base::in | std::ios_base::binary);
    std::stringstream stream;
    stream << in->rdbuf();
    std::string contents = stream.str();
    const char* data = contents.data();

    // Cannot use c_str to copy because it will truncate on first \0 character
    // which might appear multiple times in binary data
    memcpy(buffer, data, contents.size());

    return assets_[it->second].size();
}

unsigned int ZDevResourceFile::ResourceCount() const
{
    return assets_.size();
}

std::string ZDevResourceFile::ResourceName(unsigned int num) const
{
    return assets_[num].fileName();
}

void ZDevResourceFile::PrintResources() const
{
    for (auto it = assetIndices_.begin(), end = assetIndices_.end(); it != end; it++)
    {
        LOG(assets_[it->second].path(), ZSeverity::Error);
    }
}

void ZDevResourceFile::Close()
{
    assetIndices_.clear();
    assets_.clear();
}

void ZDevResourceFile::ReadDirectoryEntries(const FileHandle& fh)
{
    for (FileIterator it = fh.begin(); it != fh.end(); ++it)
    {
        FileHandle file = fs::open(fh.path() + "/" + *it);
        if (file.isDirectory())
        {
            ReadDirectoryEntries(file);
        }
        else
        {
            auto fp = file.path();
            fp = fp.replace(fp.find(fileName_), fileName_.size(), "");
            assetIndices_[fp] = assets_.size();
            assets_.push_back(file);
        }
    }
}
