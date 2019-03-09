//
//  ZResourceFile.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZResource.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZResourceFile {

public:

  virtual ~ZResourceFile() { }
  virtual bool Open() = 0;
  virtual unsigned int RawResourceSize(ZResource& resource) = 0;
  virtual unsigned int RawResource(ZResource& resource, char* buffer) = 0;
  virtual unsigned int ResourceCount() const = 0;
  virtual std::string ResourceName(unsigned int num) const = 0;
  virtual void Close() = 0;

protected:

};
