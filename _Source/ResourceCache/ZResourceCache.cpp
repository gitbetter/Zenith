//
//  ZResourceCache.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 08/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZResourceCache.hpp"
#include "ZDefaultResourceLoader.hpp"

ZResourceCache::ZResourceCache(const unsigned int sizeInMb, std::shared_ptr<ZResourceFile> resourceFile) {
  cacheSize_ = sizeInMb;
  allocated_ = 0;
  file_ = resourceFile;
}

ZResourceCache::~ZResourceCache() {
  while (!lru_.empty()) FreeOneResource();
  file_ = nullptr;
}

bool ZResourceCache::Init() {
  bool success = false;
  if (file_->Open()) {
    RegisterLoader(std::shared_ptr<ZResourceLoader>(new ZDefaultResourceLoader));
    success = true;
  }
  return success;
}

void ZResourceCache::RegisterLoader(std::shared_ptr<ZResourceLoader> loader) {
  if (loader) resourceLoaders_.push_front(loader);
}

std::shared_ptr<ZResourceHandle> ZResourceCache::GetHandle(ZResource* resource) {
  std::shared_ptr<ZResourceHandle> handle(Find(resource));
  if (handle == nullptr) handle = Load(resource);
  else Update(handle);
  return handle;
}

int ZResourceCache::Preload(const std::string pattern, void (*progressCallback)(int, bool &)) {
  // TODO: Could be used to load level specific or pinch point
  // assets (by pattern matching a specific folder)
  return 0;
}

void ZResourceCache::Flush() {
  lru_.clear();
}

std::shared_ptr<ZResourceHandle> ZResourceCache::Find(ZResource* resource) {
  if (resources_.find(resource->name) != resources_.end())
    return resources_[resource->name];
  return nullptr;
}

const void ZResourceCache::Update(std::shared_ptr<ZResourceHandle> handle) {
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

  for (ResourceLoaderList::iterator it = resourceLoaders_.begin(); it != resourceLoaders_.end(); it++) {
    std::shared_ptr<ZResourceLoader> testLoader = *it;
    if (MatchPattern(testLoader->Pattern(), resource->name)) {
      loader = testLoader; break;
    }
  }

  if (!loader) {
    _Z("Default resource loader not found!", ZERROR);
    return handle;
  }

  unsigned int rawSize = file_->RawResourceSize(*resource);
  char* rawBuffer = loader->UseRawFile() ? Allocate(rawSize) : new char[rawSize];

  if (rawBuffer == nullptr) {
    return nullptr;
  }

  file_->RawResource(*resource, rawBuffer);

  char* buffer = nullptr;
  unsigned int size = 0;

  if (loader->UseRawFile()) {
    buffer = rawBuffer;
    handle = std::shared_ptr<ZResourceHandle>(new ZResourceHandle(*resource, buffer, rawSize, this));
  } else {
    size = loader->LoadedResourceSize(rawBuffer, rawSize);
    buffer = Allocate(size);

    if (rawBuffer == nullptr || buffer == nullptr) return nullptr;

    handle = std::shared_ptr<ZResourceHandle>(new ZResourceHandle(*resource, buffer, size, this));
    bool success = loader->LoadResource(rawBuffer, rawSize, handle);

    delete[] rawBuffer;

    if (!success) return nullptr;
  }

  if (handle) {
    lru_.push_front(handle);
    resources_[resource->name] = handle;
  }

  return handle;
}

void ZResourceCache::Free(std::shared_ptr<ZResourceHandle> handle) {
  lru_.remove(handle);
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
  if (!MakeRoom(size)) return nullptr;

  char* mem = new char[size];
  if (mem) allocated_ += size;

  return mem;
}

void ZResourceCache::FreeOneResource() {
  ResourceHandleList::iterator removed = lru_.end();
  removed--;

  std::shared_ptr<ZResourceHandle> handle = *removed;

  lru_.pop_back();
  resources_.erase(handle->resource_.name);
}

void ZResourceCache::FreeMemory(unsigned int size) {
  allocated_ -= size;
}
