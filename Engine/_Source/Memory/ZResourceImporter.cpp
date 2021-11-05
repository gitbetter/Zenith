/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZResourceImporter.cpp

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

#include "ZResourceImporter.hpp"
#include "ZScriptResourceLoader.hpp"
#include "ZWavResourceLoader.hpp"
#include "ZOggResourceLoader.hpp"
#include "ZModelResourceLoader.hpp"
#include "ZTextureResourceLoader.hpp"
#include "ZZofResourceLoader.hpp"
#include "ZShaderResourceLoader.hpp"
#include "ZFontResourceLoader.hpp"
#include "ZResourceLoadTask.hpp"
#include "ZServices.hpp"

ZResourceImporter::~ZResourceImporter()
{
    for (ResourceFileMap::iterator it = resourceFiles_.begin(); it != resourceFiles_.end(); it++)
    {
        it->second->Close();
    }
    resourceFiles_.clear();
}

void ZResourceImporter::Initialize()
{
	RegisterLoader(std::shared_ptr<ZScriptResourceLoader>(new ZScriptResourceLoader));
	RegisterLoader(std::shared_ptr<ZWavResourceLoader>(new ZWavResourceLoader));
	RegisterLoader(std::shared_ptr<ZOggResourceLoader>(new ZOggResourceLoader));
    RegisterLoader(std::shared_ptr<ZModelResourceLoader>(new ZModelResourceLoader));
    RegisterLoader(std::shared_ptr<ZTextureResourceLoader>(new ZTextureResourceLoader));
    RegisterLoader(std::shared_ptr<ZZofResourceLoader>(new ZZofResourceLoader));
	RegisterLoader(std::shared_ptr<ZShaderResourceLoader>(new ZShaderResourceLoader));
	RegisterLoader(std::shared_ptr<ZFontResourceLoader>(new ZFontResourceLoader));
}

void ZResourceImporter::RegisterResourceFile(std::shared_ptr<ZResourceFile> file)
{
    std::lock_guard<std::mutex> lock(mutexes_.resourceFiles);
    if (resourceFiles_.find(file->Name()) != resourceFiles_.end()) return;
    if (file->Open())
    {
        resourceFiles_[file->Name()] = file;
    }
    else
    {
        LOG("Could not open resource file " + file->Name(), ZSeverity::Error);
    }
}

void ZResourceImporter::RegisterLoader(std::shared_ptr<ZIResourceLoader> loader)
{
	if (loader)
	{
		std::lock_guard<std::mutex> lock(mutexes_.resourceLoaders);
		resourceLoaders_.push_front(loader);
	}
}

void ZResourceImporter::GetData(ZResourceData* resource)
{
	std::lock_guard<std::mutex> lock(mutexes_.all);
	Load(resource);
}

void ZResourceImporter::GetDataAsync(const std::shared_ptr<ZResourceData>& resource)
{
	std::shared_ptr<ZResourceLoadTask> loadTask = std::make_shared<ZResourceLoadTask>(resource);
	loadTask->Start();
}

bool ZResourceImporter::MatchPattern(const std::string& pattern, const std::string& str)
{
    std::regex rx(pattern);
    return std::regex_match(str, rx);
}

void ZResourceImporter::Load(ZResourceData* resource)
{
	std::shared_ptr<ZIResourceLoader> loader;

	for (ResourceLoaderList::iterator it = resourceLoaders_.begin(); it != resourceLoaders_.end(); it++)
	{
		std::shared_ptr<ZIResourceLoader> testLoader = *it;
		if (MatchPattern(testLoader->Pattern(), resource->path))
		{
			loader = testLoader; break;
		}
	}

	if (!loader)
	{
		LOG("Default resource loader not found for resource " + resource->path + "!", ZSeverity::Error);
		return;
	}

	// Gets the first resource file in the resource files list that contains the
	// given resource (indicated by return size greater than 0)
	unsigned int rawSize = 1;
	ResourceFileMap::iterator it;

	for (it = resourceFiles_.begin(); it != resourceFiles_.end(); it++)
	{
		rawSize += it->second->RawResourceSize(resource->path);
		if (rawSize > 1) break;
	}

	char* rawBuffer = new char[rawSize];

	if (rawBuffer == nullptr)
	{
		return;
	}

	if (it != resourceFiles_.end())
		it->second->RawResource(resource->path, rawBuffer);

	rawBuffer[rawSize - 1] = '\0';

	if (loader->UseRawFile())
	{
		resource->buffer = static_cast<void*>(rawBuffer);
		resource->size = rawSize;
	}
	else
	{
		unsigned int size = loader->LoadedResourceSize(rawBuffer, rawSize);
		char* buffer = new char[size];

		if (rawBuffer == nullptr || buffer == nullptr) return;

		resource->buffer = static_cast<void*>(buffer);
		resource->size = size;

		bool success = loader->LoadResource(rawBuffer, rawSize, resource);

		delete[] rawBuffer;

		if (!success) return;
	}
}