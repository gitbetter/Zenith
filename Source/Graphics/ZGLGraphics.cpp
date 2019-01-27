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

ZGLGraphics::ZGLGraphics(int windowWidth, int windowHeight) {
  window_ = new ZGLWindow(windowWidth, windowHeight);
}

ZGLGraphics::~ZGLGraphics() {
  Delete();
}

void ZGLGraphics::Draw(const std::vector<ZGameObject*>& gameObjects, float frameMix) {
  if (!window_->WindowShouldClose()) {
    glClearColor(0.3f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (unsigned int i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Render(frameMix);
    }

    window_->SwapBuffers();
  }
}

void ZGLGraphics::Delete() {
  window_->Destroy();
  delete window_;
  glfwTerminate();
}

const ZWindow* ZGLGraphics::GetWindow() {
  return window_;
};
