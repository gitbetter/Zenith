//
//  ZDefaultResourceLoader.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZResourceLoader.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZDefaultResourceLoader : public ZResourceLoader {

public:

    ~ZDefaultResourceLoader() { }
    std::string Pattern() override { return "*"; }
    bool UseRawFile() override { return true; };
    unsigned int LoadedResourceSize(char* rawBuffer, unsigned int rawSize) override { return rawSize; }
    bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ZResourceHandle> handle) override { return true; }

protected:

};
