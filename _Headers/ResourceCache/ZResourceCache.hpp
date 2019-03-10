//
//  ZResourceCache.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 08/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZResourceHandle.hpp"
#include "ZResourceLoader.hpp"
#include "ZResourceFile.hpp"

// Forward Declarations
class ZResource;

// Class and Data Structure Definitions
typedef std::list<std::shared_ptr<ZResourceHandle>> ResourceHandleList;
typedef std::map<std::string, std::shared_ptr<ZResourceHandle>> ResourceHandleMap;
typedef std::map<std::string, std::shared_ptr<ZResourceFile>> ResourceFileMap;
typedef std::list<std::shared_ptr<ZResourceLoader>> ResourceLoaderList;

class ZResourceCache {

  friend class ZResourceHandle;

public:

  ZResourceCache(const unsigned int sizeInMb);
  ~ZResourceCache();

  bool Initialize();
  void RegisterLoader(std::shared_ptr<ZResourceLoader> loader);
  void RegisterResourceFile(std::shared_ptr<ZResourceFile> file);
  void SetActiveResourceFile(std::shared_ptr<ZResourceFile> file);
  std::shared_ptr<ZResourceHandle> GetHandle(ZResource* resource);
  int Preload(const std::string pattern, void (*progressCallback)(int, bool &));
  void Flush();

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
  bool MatchPattern(std::string pattern, std::string str);

  bool MakeRoom(unsigned int size);
  char* Allocate(unsigned int size);
  void FreeOneResource();
  void FreeMemory(unsigned int size);
};
