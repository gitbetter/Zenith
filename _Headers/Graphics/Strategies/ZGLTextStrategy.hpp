//
//  ZGLTextStrategyStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZTextStrategy.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZGLTextStrategy : public ZTextStrategy {
private:

public:

  ZGLTextStrategy() { }
  ~ZGLTextStrategy() { }

  void LoadFont(std::string fontPath, unsigned int fontSize) override;
protected:


};
