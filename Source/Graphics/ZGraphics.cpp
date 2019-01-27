//
//  ZGLGraphics.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGLGraphics.hpp"
#include "ZLogger.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

ZGLGraphics::ZGraphics(int windowWidth, int windowHeight) {
  glfwSetErrorCallback(GLFWErrorCallback);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window_ = glfwCreateWindow(windowWidth, windowHeight, "Starter", NULL, NULL);
  if (window_ == NULL) {
      ZLogger::Log("Could not create glfw window", ZLoggerSeverity::Error);
      glfwTerminate();
  }
  glfwMakeContextCurrent(window_);

  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
}

void ZGLGraphics::Draw() override {
  if (!glfwWindowShouldClose(window)) {
    glClearColor(0.3f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void ZGLGraphics::Delete() override {
  glfwTerminate();
}

void ZGLGraphics::GLFWErrorCallback(int id, const char* description) {
  ZLogger::Log(description, ZLoggerSeverity::Error);
}
