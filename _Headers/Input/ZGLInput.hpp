//
//  ZGLInput.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZInput.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZGLInput : public ZInput {
private:
  double lastPitch_, lastYaw_;
  bool firstLook_ = true;

public:
  void Process() override;
protected:

};
