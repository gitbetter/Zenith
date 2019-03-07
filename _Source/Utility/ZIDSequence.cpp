//
//  ZIDSequence.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZIDSequence.hpp"

ZIDSequence::ZIDSequence(std::string seed) {
  std::seed_seq seedSeq(seed.begin(), seed.end());
  generator_.seed(seedSeq);
}
