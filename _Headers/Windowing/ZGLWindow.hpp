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
struct GLFWwindow;

// Class and Data Structure Definitions
class ZGLWindow : public ZWindow {
private:
  GLFWwindow* window_ = nullptr;

  static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
  void Initialize();

public:
  ZGLWindow(int width, int height);
  virtual ~ZGLWindow() { }

  virtual void CloseWindow() const override;
  virtual bool WindowShouldClose() const override;
  virtual void PollEvents() const override;
  virtual void Destroy() override;

  GLFWwindow* GetHandle() const { return window_; }
protected:

};
