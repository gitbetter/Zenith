/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourceCache.hpp

    Created by Adrian Sanchez on 08/03/2019.
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

#pragma once

// Includes
#include "ZResourceHandle.hpp"
#include "ZResourceLoader.hpp"
#include "ZResourceFile.hpp"

// Forward Declarations
class ZResource;

// Class and Data Structure Definitions
using ResourceHandleList = std::list<std::shared_ptr<ZResourceHandle>>;
using ResourceHandleMap = std::map<std::string, std::shared_ptr<ZResourceHandle>>;
using ResourceFileMap = std::map<std::string, std::shared_ptr<ZResourceFile>>;
using ResourceLoaderList = std::list<std::shared_ptr<ZResourceLoader>>;

class ZResourceCache
{

private:

    struct
    {
        std::mutex allocated;
        std::mutex lru;
        std::mutex resources;
        std::mutex resourceFiles;
        std::mutex resourceLoaders;
        std::mutex all;
    } mutexes_;

public:

    ZResourceCache(const unsigned int sizeInMb);
    ~ZResourceCache();

    void Initialize();
    void CleanUp() { }

    void RegisterLoader(std::shared_ptr<ZResourceLoader> loader);
    void RegisterResourceFile(std::shared_ptr<ZResourceFile> file);
    std::shared_ptr<ZResourceHandle> GetHandle(ZResource* resource);
    void RequestHandle(const ZResource& resource);
    int Preload(const std::string& pattern, void(*progressCallback)(int, bool&));
    void Flush();
    void FreeMemory(unsigned int size);

protected:

    ResourceHandleList lru_;
    ResourceHandleMap resources_;
    ResourceLoaderList resourceLoaders_;
    ResourceFileMap resourceFiles_;

    unsigned int cacheSize_;
    unsigned int allocated_;

    std::shared_ptr<ZResourceHandle> Find(ZResource* resource);
    const void Update(std::shared_ptr<ZResourceHandle> handle);
    std::shared_ptr<ZResourceHandle> Load(ZResource* resource);
    void Free(std::shared_ptr<ZResourceHandle> handle);
    bool MatchPattern(const std::string& pattern, const std::string& str);

    bool MakeRoom(unsigned int size);
    char* Allocate(unsigned int size);
    void FreeOneResource();
};
