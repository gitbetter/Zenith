/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourceImporter.hpp

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

#include "ZCommon.hpp"
#include "ZResourceData.hpp"
#include "ZResourceLoader.hpp"
#include "ZResourceFile.hpp"

using ResourceFileMap = std::map<std::string, std::shared_ptr<ZResourceFile>>;
using ResourceLoaderList = std::list<std::shared_ptr<ZIResourceLoader>>;

class ZResourceImporter
{

public:

    ZResourceImporter() = default;
    ~ZResourceImporter();

    void Initialize();
    void CleanUp() { }

    void RegisterResourceFile(std::shared_ptr<ZResourceFile> file);
	void RegisterLoader(std::shared_ptr<ZIResourceLoader> loader);

    void GetData(ZResourceData* resource);
    void GetDataAsync(const std::shared_ptr<ZResourceData>& resource);

protected:

    ResourceLoaderList resourceLoaders_;
    ResourceFileMap resourceFiles_;

protected:

    void Load(ZResourceData* resource);
    bool MatchPattern(const std::string& pattern, const std::string& str);

private:

    struct
    {
        std::mutex resources;
        std::mutex resourceFiles;
        std::mutex resourceLoaders;
        std::mutex all;
    } mutexes_;

};
