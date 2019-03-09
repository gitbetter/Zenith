//
//  ZZipFile.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZZipFile.hpp"

bool ZZipFile::Open(const std::string& file) {
  int error = 0;
  zipFile_ = zip_open(file.c_str(), ZIP_RDONLY, &error);
  fileName_ = file;

  if (error) { LogZipError(error); return false; }

  zip_stat_init(&stats_);
  zip_stat(zipFile_, fileName_.c_str(), 0, &stats_);

  return true;
}

unsigned int ZZipFile::RawResourceSize(ZResource& resource) {
  return zipFile_ ? stats_.size : 0;
}

unsigned int ZZipFile::RawResource(ZResource& resource, char* buffer) {
  if (zipFile_) {
    zip_file* f = zip_fopen(zipFile_, fileName_.c_str(), 0);
    unsigned int bytesRead = zip_fread(f, buffer, stats_.size);
    zip_fclose(f);
    return bytesRead;
  }
  return 0;
}

unsigned int ZZipFile::ResourceCount() const {
  return zipFile_ ? zip_get_num_entries(zipFile_, 0) : 0;
}

std::string ZZipFile::ResourceName(unsigned int num) const {
  return zipFile_ ? std::string(zip_get_name(zipFile_, num, 0)) : "";
}

void ZZipFile::Close() {
  if (zipFile_) zip_close(zipFile_);
}

void ZZipFile::LogZipError(int errorCode) {
  zip_error err;
  zip_error_init_with_code(&err, errorCode);
  const char* errorString = zip_error_strerror(&err);
  if (errorString)
    _Z(std::string(errorString), ZERROR);
}
