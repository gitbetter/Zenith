//
//  ZResourceHandle.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZResource.hpp"

// Forward Declarations
class ZResourceCache;

// Class and Data Structure Definitions
class ZResourceHandle {

  friend class ZResourceCache;

private:

public:

    ZResourceHandle(ZResource& resource, char* buffer, unsigned int size, ZResourceCache* resourceCache);
    virtual ~ZResourceHandle();

    unsigned int Size() const { return size_; }
    char* Buffer() const { return buffer_; }
    char* FluidBuffer() { return buffer_; }

protected:

  ZResource resource_;
  char* buffer_;
  unsigned int size_;
  ZResourceCache* resourceCache_;
  // TODO: More fields for extra data

};
