//
//  ZGLDomainStrategy.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 07/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGLDomainStrategy.hpp"
#include "ZCommon.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void ZGLDomainStrategy::Initialize() {
  glfwSetErrorCallback(GLFWErrorCallback);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

}

void ZGLDomainStrategy::CreateWindow(int width, int height) {
  GLFWwindow* window = glfwCreateWindow(width, height, "Starter", NULL, NULL);
  if (window == NULL) {
      _Z("Could not create glfw window", ZERROR);
      glfwTerminate();
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  glewInit();

  glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
  CaptureCursor();
}

void ZGLDomainStrategy::PollEvents() {
  glfwPollEvents();
}

void ZGLDomainStrategy::CaptureCursor() {
  GLFWwindow* glWindow = glfwGetCurrentContext();
  glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void ZGLDomainStrategy::ReleaseCursor() {
  GLFWwindow* glWindow = glfwGetCurrentContext();
  glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void ZGLDomainStrategy::Resize(int width, int height) {
  GLFWwindow* glWindow = glfwGetCurrentContext();
  glfwSetWindowSize(glWindow, width, height);
}

void ZGLDomainStrategy::CloseWindow() {
  GLFWwindow* glWindow = glfwGetCurrentContext();
  glfwSetWindowShouldClose(glWindow, GL_TRUE);
}

bool ZGLDomainStrategy::IsWindowClosing() {
  GLFWwindow* glWindow = glfwGetCurrentContext();
  return glfwWindowShouldClose(glWindow);
}

void ZGLDomainStrategy::CleanUp() {
  GLFWwindow* glWindow = glfwGetCurrentContext();
  glfwDestroyWindow(glWindow);
}

void ZGLDomainStrategy::GLFWErrorCallback(int id, const char* description) {
  _Z(description, ZERROR);
}

void ZGLDomainStrategy::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
