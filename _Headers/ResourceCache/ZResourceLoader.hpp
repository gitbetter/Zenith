//
//  ZResourceLoader.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZResourceHandle.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZResourceLoader {

public:

    virtual ~ZResourceLoader() { }
    virtual std::string Pattern() = 0;
    virtual bool UseRawFile() = 0;
    virtual unsigned int LoadedResourceSize(char* rawBuffer, unsigned int rawSize) = 0;
    virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ZResourceHandle> handle) = 0;

};
