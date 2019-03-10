//
//  ZZipFile.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZResourceFile.hpp"
#include "zip.h"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZZipFile : public ZResourceFile {

private:

  struct zip_t* zipFile_ = nullptr;
  std::string fileName_;

public:

    ZZipFile(const std::string& filePath) : fileName_(filePath) { }
    ~ZZipFile() { }
    bool Open() override;
    const std::string& Name() const override { return fileName_; }
    unsigned int RawResourceSize(ZResource& resource) override;
    unsigned int RawResource(ZResource& resource, char* buffer) override;
    unsigned int ResourceCount() const override;
    std::string ResourceName(unsigned int num) const override;
    void Close() override;

protected:

};
