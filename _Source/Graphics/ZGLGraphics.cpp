//
//  ZGLGraphics.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGLGraphics.hpp"
#include "ZGLWindow.hpp"
#include "ZLogger.hpp"
#include "ZGameObject.hpp"
#include "ZShader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

ZGLGraphics::ZGLGraphics(int windowWidth, int windowHeight) : ZGraphics() {
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

  GenerateDepthMap();
}

void ZGLGraphics::Draw(const std::vector<ZGameObject*>& gameObjects, const std::vector<ZLight*>& gameLights, float frameMix) {
  if (!window_->WindowShouldClose()) {
    glClearColor(0.3f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    UpdateWindowSize();

    glStencilMask(0x00);

    for (ZLight* light : gameLights) {
      DrawDepthMap(gameObjects, light, frameMix);
    }

    Render(gameObjects, frameMix);

    SwapBuffers();
  }
}

void ZGLGraphics::Render(const std::vector<ZGameObject*>& gameObjects, float frameMix, unsigned char renderOp) {
  for (unsigned int i = 0; i < gameObjects.size(); i++) {
      gameObjects[i]->Render(frameMix, renderOp);
  }
}

void ZGLGraphics::SwapBuffers() {
  ZGLWindow* glWindow = dynamic_cast<ZGLWindow*>(window_);
  assert(glWindow);
  glfwSwapBuffers(glWindow->GetHandle());
}

void ZGLGraphics::EnableStencilBuffer() {
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0xFF);
}

void ZGLGraphics::DisableStencilBuffer() {
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilMask(0x00);
}

void ZGLGraphics::GenerateDepthMap() {
  glGenFramebuffers(1, &depthFramebuffer_);
  glGenTextures(1, &depthMap_);
  glBindTexture(GL_TEXTURE_2D, depthMap_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ZEngine::SHADOW_MAP_WIDTH, ZEngine::SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLGraphics::DrawDepthMap(const std::vector<ZGameObject*>& gameObjects, ZLight* light, float frameMix) {
  if (shadowShader_ == nullptr) {
    shadowShader_ = new ZShader("Resources/Shaders/Vertex/shadow.vert", "Resources/Shaders/Pixel/shadow.frag");
  }
  shadowShader_->Activate();
  float near = 1.f, far = 7.5f;
  glm::mat4 lightP = glm::ortho(-10.f, 10.f, -10.f, 10.f, near, far);
  glm::mat4 lightV = glm::lookAt(light->GetPosition(), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
  glm::mat4 lightSpaceMatrix = lightP * lightV;
  // TODO: For now we support one light source for shadows, but this should change
  // so that multiple light space matrices are supported for multiple light sources
  // that can cast shadows, possibly using deferred rendering
  currentLightSpaceMatrix_ = lightSpaceMatrix;

  glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer_);
  glViewport(0, 0, ZEngine::SHADOW_MAP_WIDTH, ZEngine::SHADOW_MAP_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT);

  Render(gameObjects, frameMix, ZGraphics::RENDER_OP_DEPTH);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window_->GetWidth(), window_->GetHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ZGLGraphics::BindDepthMap() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap_);
}

void ZGLGraphics::UpdateWindowSize() {
  ZGLWindow* glWindow = dynamic_cast<ZGLWindow*>(window_);
  assert(glWindow);

  int width, height;
  glfwGetWindowSize(glWindow->GetHandle(), &width, &height);
  window_->SetSize(width, height);
}

void ZGLGraphics::Delete() {
  if (window_ != nullptr) {
    window_->Destroy();
    delete window_;
  }

  if (shadowShader_ != nullptr) {
    delete shadowShader_;
  }

  glfwTerminate();
}

void ZGLGraphics::GLFWErrorCallback(int id, const char* description) {
  ZLogger::Log(description, ZLoggerSeverity::Error);
}
