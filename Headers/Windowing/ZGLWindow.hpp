//
//  ZGLWindow.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZWindow.hpp"

// Forward Declarations
class GLFWwindow;

// Class and Data Structure Definitions
class ZGLWindow : public ZWindow {
private:
  GLFWwindow* window_ = nullptr;

  static void GLFWErrorCallback(int id, const char* description);
  void InitializeGLContext();

public:
  ZGLWindow(int width, int height);
  virtual ~ZGLWindow() { }

  virtual bool WindowShouldClose() const override;
  virtual void SwapBuffers() const override;
  virtual void Destroy() override;
protected:

};
