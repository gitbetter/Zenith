//
//  ZIDSequence.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <string>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZIDSequence {
private:
  unsigned int current_ = 0;
public:
  ZIDSequence() { }
  ~ZIDSequence() { }

  // TODO: Use a random number generator to sample from a
  // list of tokens and use a few tokens as the prefix
  std::string Next() { return std::to_string(current_++); }

protected:
};
