//
//  ZZipFile.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZZipFile.hpp"

bool ZZipFile::Open() {
  zipFile_ = zip_open(fileName_.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');

  if (!zipFile_) {
    _Z("Failed to open zip file " + fileName_, ZERROR);
    return false;
  }

  return true;
}

unsigned int ZZipFile::RawResourceSize(ZResource& resource) {
  return zipFile_ ? zip_entry_size(zipFile_) : 0;
}

unsigned int ZZipFile::RawResource(ZResource& resource, char* buffer) {
  if (zipFile_) {
    size_t bytes;
    zip_entry_open(zipFile_, fileName_.c_str());
    zip_entry_read(zipFile_, (void **)buffer, &bytes);
    zip_entry_close(zipFile_);
    return (unsigned int)bytes;
  }
  return 0;
}

unsigned int ZZipFile::ResourceCount() const {
  return zipFile_ ? zip_total_entries(zipFile_) : 0;
}

std::string ZZipFile::ResourceName(unsigned int num) const {
  zip_entry_openbyindex(zipFile_, num);
  std::string name = std::string(zip_entry_name(zipFile_));
  zip_entry_close(zipFile_);
  return name;
}

void ZZipFile::Close() {
  if (zipFile_) zip_close(zipFile_);
}
