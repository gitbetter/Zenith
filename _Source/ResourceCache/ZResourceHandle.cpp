//
//  ZResourceHandle.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZResourceHandle.hpp"

ZResourceHandle::ZResourceHandle(ZResource& resource, char* buffer, unsigned int size, ZResourceCache* resourceCache) : resource_(resource), buffer_(buffer), size_(size), resourceCache_(resourceCache) {  }

ZResourceHandle::~ZResourceHandle() {
  delete[] buffer_;
  // TODO: resourceCache_->MemoryHasBeenFreed(size_);
}
