//
//  ZGLDomainStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 07/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZDomainStrategy.hpp"

// Forward Declarations
struct GLFWwindow;

// Class and Data Structure Definitions
class ZGLDomainStrategy : public ZDomainStrategy {
private:

  static void GLFWErrorCallback(int id, const char* description);
  static void FrameBufferSizeCallback(GLFWwindow* window, int height, int width);

public:

  ZGLDomainStrategy() { }
  ~ZGLDomainStrategy() { }

  void Initialize() override;
  void CreateWindow(int width, int height) override;
  void PollEvents() override;
  void CaptureCursor() override;
  void ReleaseCursor() override;
  void Resize(int width, int height) override;
  glm::vec2 FramebufferSize() override;
  void CloseWindow() override;
  bool IsWindowClosing() override;
  void CleanUp() override;
protected:


};
