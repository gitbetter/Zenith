//
//  ZGLWindow.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGLWindow.hpp"
#include "ZLogger.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

ZGLWindow::ZGLWindow(int width, int height) : ZWindow(width, height) {
  InitializeGLContext();
}

void ZGLWindow::InitializeGLContext() {
  glfwSetErrorCallback(GLFWErrorCallback);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window_ = glfwCreateWindow(width_, height_, "Starter", NULL, NULL);
  if (window_ == NULL) {
      ZLogger::Log("Could not create glfw window", ZLoggerSeverity::Error);
      glfwTerminate();
  }
  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, FrameBufferSizeCallback);
  //glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
}

void ZGLWindow::SwapBuffers() const {
  glfwSwapBuffers(window_);
}

void ZGLWindow::PollEvents() const {
  glfwPollEvents();
}

bool ZGLWindow::WindowShouldClose() const {
  return glfwWindowShouldClose(window_);
}

void ZGLWindow::Destroy() {
  if (window_ != nullptr) glfwDestroyWindow(window_);
}

void ZGLWindow::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void ZGLWindow::GLFWErrorCallback(int id, const char* description) {
  ZLogger::Log(description, ZLoggerSeverity::Error);
}
