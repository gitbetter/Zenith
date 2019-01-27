//
//  ZGLGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphics.hpp"

// Forward Declarations
class GLFWwindow;

// Class and Data Structure Definitions
class ZGLGraphics : public ZGraphics {
private:
  GLFWwindow* window_;

  void GLFWErrorCallback(int id, const char* description);

public:
    ZGLGraphics(int windowWidth, int windowHeight);
    ~ZGLGraphics() { }

    virtual void Draw() override;
    virtual void Delete() override;

protected:

};
