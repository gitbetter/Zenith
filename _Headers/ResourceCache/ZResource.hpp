//
//  ZResource.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 08/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZResource {

public:

  std::string name;

  ZResource(const std::string& name);
  ~ZResource() { }

};
