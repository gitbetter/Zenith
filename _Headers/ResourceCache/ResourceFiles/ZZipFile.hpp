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
#include <zip.h>

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZZipFile : public ZResourceFile {

private:

  zip* zipFile_;
  std::string fileName_;
  struct zip_stat stats_;

  void LogZipError(int errorCode);

public:

    ZZipFile(const std::string& filePath) : fileName_(filePath) { }
    ~ZZipFile() { }
    bool Open() override;
    unsigned int RawResourceSize(ZResource& resource) override;
    unsigned int RawResource(ZResource& resource, char* buffer) override;
    unsigned int ResourceCount() const override;
    std::string ResourceName(unsigned int num) const override;
    void Close() override;

protected:

};
