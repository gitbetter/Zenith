//
//  ZGLGraphics.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGLGraphics.hpp"
#include "ZGLWindow.hpp"
#include "ZLogger.hpp"
#include "ZGameObject.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cassert>

ZGLGraphics::ZGLGraphics(int windowWidth, int windowHeight) {
  Initialize(windowWidth, windowHeight);
}

ZGLGraphics::~ZGLGraphics() {
  Delete();
}

void ZGLGraphics::Initialize(int windowWidth, int windowHeight) {
  glfwSetErrorCallback(GLFWErrorCallback);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window_ = new ZGLWindow(windowWidth, windowHeight);

  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void ZGLGraphics::Draw(const std::vector<ZGameObject*>& gameObjects, float frameMix) {
  if (!window_->WindowShouldClose()) {
    glClearColor(0.3f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    UpdateWindowSize();

    for (unsigned int i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Render(frameMix);
    }

    SwapBuffers();
  }
}

void ZGLGraphics::SwapBuffers() const {
  ZGLWindow* glWindow = dynamic_cast<ZGLWindow*>(window_);
  assert(glWindow);
  glfwSwapBuffers(glWindow->GetHandle());
}

void ZGLGraphics::UpdateWindowSize() {
  ZGLWindow* glWindow = dynamic_cast<ZGLWindow*>(window_);
  assert(glWindow);

  int width, height;
  glfwGetWindowSize(glWindow->GetHandle(), &width, &height);
  window_->SetSize(width, height);
}

void ZGLGraphics::Delete() {
  window_->Destroy();
  delete window_;
  glfwTerminate();
}

void ZGLGraphics::GLFWErrorCallback(int id, const char* description) {
  ZLogger::Log(description, ZLoggerSeverity::Error);
}
