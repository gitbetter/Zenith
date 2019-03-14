//
//  ZGLInput.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZInput.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZGLInput : public ZInput {

private:

  double lastPitch_, lastYaw_;
  bool firstLook_ = true;
  bool keyPressState_[GLFW_KEY_LAST] = { false };
  bool mousePressState_[GLFW_MOUSE_BUTTON_8] = { false };

public:

  void Process() override;
  void GetPointerPosition(double& x, double& y) override;

protected:

};
