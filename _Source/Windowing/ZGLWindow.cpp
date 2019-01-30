//
//  ZGLWindow.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGLWindow.hpp"
#include "ZLogger.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

ZGLWindow::ZGLWindow(int width, int height) : ZWindow(width, height) {
  Initialize();
}

void ZGLWindow::Initialize() {
  window_ = glfwCreateWindow(width_, height_, "Starter", NULL, NULL);
  if (window_ == NULL) {
      ZLogger::Log("Could not create glfw window", ZLoggerSeverity::Error);
      glfwTerminate();
  }
  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, FrameBufferSizeCallback);
}

void ZGLWindow::PollEvents() const {
  glfwPollEvents();
}

bool ZGLWindow::WindowShouldClose() const {
  return glfwWindowShouldClose(window_);
}

void ZGLWindow::Destroy() {
  glfwDestroyWindow(window_);
}

void ZGLWindow::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
