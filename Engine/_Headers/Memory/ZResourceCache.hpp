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

#include "ZResourceHandle.hpp"
#include "ZResourceLoader.hpp"
#include "ZResourceFile.hpp"

class ZResource;

using ResourceHandleList = std::list<std::shared_ptr<ZResourceHandle>>;
using ResourceHandleMap = std::map<std::string, std::shared_ptr<ZResourceHandle>>;
using ResourceFileMap = std::map<std::string, std::shared_ptr<ZResourceFile>>;
using ResourceLoaderList = std::list<std::shared_ptr<ZResourceLoader>>;


// TODO: Deprecate! Should only take care of loading resources
// New class name: ZResourceImporter
class ZResourceCache
{

private:

    struct
    {
        std::mutex resources;
        std::mutex resourceFiles;
        std::mutex resourceLoaders;
        std::mutex all;
        // TODO: Deprecate!
		std::mutex allocated;
		std::mutex lru;
    } mutexes_;

public:

    ZResourceCache(const unsigned int sizeInMb);
    ~ZResourceCache();

    void Initialize();
    void CleanUp() { }

    void RegisterLoader(std::shared_ptr<ZResourceLoader> loader);
    void RegisterResourceFile(std::shared_ptr<ZResourceFile> file);
    std::shared_ptr<ZResourceHandle> GetHandle(ZResource* resource);
    void GetHandleAsync(const ZResource& resource);

    // TODO: Deprecate!
    int Preload(const std::string& pattern, void(*progressCallback)(int, bool&));
    void Flush();
    void FreeMemory(unsigned int size);

protected:

    ResourceLoaderList resourceLoaders_;
    ResourceFileMap resourceFiles_;

    std::shared_ptr<ZResourceHandle> Load(ZResource* resource);
    bool MatchPattern(const std::string& pattern, const std::string& str);

    // TODO: Deprecate!
    ResourceHandleList lru_;
    ResourceHandleMap resources_;
    // TODO: Deprecate!
    unsigned int cacheSize_;
    unsigned int allocated_;

    // TODO: Deprecate!
    std::shared_ptr<ZResourceHandle> Find(ZResource* resource);
    const void Update(std::shared_ptr<ZResourceHandle> handle);
    void Free(std::shared_ptr<ZResourceHandle> handle);
    bool MakeRoom(unsigned int size);
    char* Allocate(unsigned int size);
    void FreeOneResource();
};
