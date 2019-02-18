//
//  ZOFTree.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
struct ZOFTree {
  std::vector<ZOFObjectNode*> children;
};

struct ZOFObjectNode {
  std::vector<ZOFObjectNode*> children;
  std::vector<ZOFPropertyNode*> properties;
};

struct ZOFPropertyNode {
  std::string key;
  std::vector<ZOFValueTerminal*> values;
};

template<typename T>
struct ZOFValueTerminal {
  T value;
};
