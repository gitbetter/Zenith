//
//  ZIDSequence.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include <random>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZIDSequence {
private:

  std::mt19937 generator_;

public:

  ZIDSequence() { }
  ZIDSequence(std::string seed);
  ~ZIDSequence() { }

  // TODO: Use a random number generator to sample from a
  // list of tokens and use a few tokens as the prefix
  std::string Next() { return std::to_string(generator_() % 100000); }

protected:

};
