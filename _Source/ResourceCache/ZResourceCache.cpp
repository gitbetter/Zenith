/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZResourceCache.cpp

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

#include "ZResourceCache.hpp"
#include "ZDefaultResourceLoader.hpp"

ZResourceCache::ZResourceCache(const unsigned int sizeInMb) {
	cacheSize_ = sizeInMb * 1024 * 1024;
	allocated_ = 0;
}

ZResourceCache::~ZResourceCache() {
	std::lock_guard<std::mutex>(mutexes_.lru);
	std::lock_guard<std::mutex>(mutexes_.resourceFiles);

	while (!lru_.empty()) FreeOneResource();
	for (ResourceFileMap::iterator it = resourceFiles_.begin(); it != resourceFiles_.end(); it++) {
		it->second->Close();
	}
	resourceFiles_.clear();
	lru_.clear();
}

bool ZResourceCache::Initialize() {
	bool success = false;
	{
		std::lock_guard<std::mutex>(mutexes_.resourceFiles);
		for (ResourceFileMap::iterator it = resourceFiles_.begin(); it != resourceFiles_.end(); it++) {
			if (it->second->Open()) {
				success = true;
			}
		}
	}

	if (success)
		RegisterLoader(std::shared_ptr<ZResourceLoader>(new ZDefaultResourceLoader));

	return success;
}

void ZResourceCache::RegisterLoader(std::shared_ptr<ZResourceLoader> loader) {
	std::lock_guard<std::mutex>(mutexes_.resourceLoaders);
	if (loader) resourceLoaders_.push_front(loader);
}

void ZResourceCache::RegisterResourceFile(std::shared_ptr<ZResourceFile> file) {
	std::lock_guard<std::mutex>(mutexes_.resourceFiles);
	if (resourceFiles_.find(file->Name()) != resourceFiles_.end()) return;
	resourceFiles_[file->Name()] = file;
}

std::shared_ptr<ZResourceHandle> ZResourceCache::GetHandle(ZResource* resource) {
	std::lock_guard<std::mutex>(mutexes_.all);
	std::shared_ptr<ZResourceHandle> handle(Find(resource));
	if (!handle) handle = Load(resource);
	else Update(handle);
	return handle;
}

int ZResourceCache::Preload(const std::string pattern, void(*progressCallback)(int, bool &)) {
	// TODO: Could be used to load level specific or pinch point
	// assets (by pattern matching a specific folder)
	return 0;
}

void ZResourceCache::Flush() {
	std::lock_guard<std::mutex>(mutexes_.lru);
	lru_.clear();
}

std::shared_ptr<ZResourceHandle> ZResourceCache::Find(ZResource* resource) {
	std::lock_guard<std::mutex>(mutexes_.resources);
	if (resources_.find(resource->name) != resources_.end())
		return resources_[resource->name];
	return std::shared_ptr<ZResourceHandle>();
}

const void ZResourceCache::Update(std::shared_ptr<ZResourceHandle> handle) {
	std::lock_guard<std::mutex>(mutexes_.lru);
	ResourceHandleList::iterator it;
	for (it = lru_.begin(); it != lru_.end(); it++) {
		if ((*it) == handle) break;
	}
	if (it == lru_.end()) return;

	std::shared_ptr<ZResourceHandle> updatedHandle(*it);
	lru_.erase(it);
	lru_.push_front(updatedHandle);
}

std::shared_ptr<ZResourceHandle> ZResourceCache::Load(ZResource* resource) {
	std::shared_ptr<ZResourceLoader> loader;
	std::shared_ptr<ZResourceHandle> handle;

	{
		std::lock_guard<std::mutex>(mutexes_.resourceLoaders);
		for (ResourceLoaderList::iterator it = resourceLoaders_.begin(); it != resourceLoaders_.end(); it++) {
			std::shared_ptr<ZResourceLoader> testLoader = *it;
			if (MatchPattern(testLoader->Pattern(), resource->name)) {
				loader = testLoader; break;
			}
		}
	}

	if (!loader) {
		_Z("Default resource loader not found!", ZERROR);
		return handle;
	}

	// Gets the first resource file in the resource files list that contains the
	// given resource (indicated by return size greater than 0)
	unsigned int rawSize = 1;
	ResourceFileMap::iterator it;

	{
		std::lock_guard<std::mutex>(mutexes_.resourceFiles);
		for (it = resourceFiles_.begin(); it != resourceFiles_.end(); it++) {
			rawSize += it->second->RawResourceSize(*resource);
			if (rawSize != 0) break;
		}
	}

	char* rawBuffer = loader->UseRawFile() ? Allocate(rawSize) : new char[rawSize];

	if (rawBuffer == nullptr) {
		return nullptr;
	}

	if (it != resourceFiles_.end())
		it->second->RawResource(*resource, rawBuffer);

	rawBuffer[rawSize - 1] = '\0';

	char* buffer = nullptr;
	unsigned int size = 0;

	if (loader->UseRawFile()) {
		buffer = rawBuffer;
		handle = std::shared_ptr<ZResourceHandle>(new ZResourceHandle(*resource, (void*)buffer, rawSize, this));
	} else {
		size = loader->LoadedResourceSize(rawBuffer, rawSize);
		buffer = Allocate(size);

		if (rawBuffer == nullptr || buffer == nullptr) return nullptr;

		handle = std::shared_ptr<ZResourceHandle>(new ZResourceHandle(*resource, (void*)buffer, size, this));
		bool success = loader->LoadResource(rawBuffer, rawSize, handle);

		delete[] rawBuffer;

		if (!success) return nullptr;
	}

	if (handle) {
		std::lock_guard<std::mutex>(mutexes_.lru);
		std::lock_guard<std::mutex>(mutexes_.resources);

		lru_.push_front(handle);
		resources_[resource->name] = handle;
	}

	return handle;
}

void ZResourceCache::Free(std::shared_ptr<ZResourceHandle> handle) {
	std::lock_guard<std::mutex>(mutexes_.lru);
	lru_.remove(handle);
	FreeMemory(handle->Size());
}

bool ZResourceCache::MatchPattern(std::string pattern, std::string str) {
	std::regex rx(pattern);
	return std::regex_match(str, rx);
}

bool ZResourceCache::MakeRoom(unsigned int size) {
	if (size > cacheSize_) return false;

	while (size > cacheSize_ - allocated_) {
		if (lru_.empty()) return false;
		FreeOneResource();
	}

	return true;
}

char* ZResourceCache::Allocate(unsigned int size) {
	std::lock_guard<std::mutex>(mutexes_.allocated);
	std::lock_guard<std::mutex>(mutexes_.lru);

	if (!MakeRoom(size)) return nullptr;

	char* mem = new char[size];
	if (mem) allocated_ += size;

	return mem;
}

void ZResourceCache::FreeOneResource() {
	ResourceHandleList::iterator removed = lru_.end();
	removed--;

	std::shared_ptr<ZResourceHandle> handle = *removed;

	std::lock_guard<std::mutex>(mutexes_.resources);
	lru_.pop_back();
	resources_.erase(handle->resource_.name);

	FreeMemory(handle->Size());
	handle->resourceCache_ = nullptr;
}

void ZResourceCache::FreeMemory(unsigned int size) {
	std::lock_guard<std::mutex>(mutexes_.allocated);
	allocated_ -= size;
}
