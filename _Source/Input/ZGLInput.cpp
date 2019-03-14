//
//  ZGLInput.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGLInput.hpp"
#include "ZEventAgent.hpp"
#include "ZObjectLookEvent.hpp"
#include "ZObjectMoveEvent.hpp"
#include "ZQuitEvent.hpp"
#include "ZFireEvent.hpp"
#include <cassert>

void ZGLInput::Process() {
  GLFWwindow* windowHandle = glfwGetCurrentContext();
  // Todo: Replace 'commands' with 'events', and use a global event managaer to dispatch
  if (glfwGetKey(windowHandle, GLFW_KEY_W) == GLFW_PRESS) {
    std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, 1.f));
    ZEngine::EventAgent()->TriggerEvent(moveEvent);
  }
  if (glfwGetKey(windowHandle, GLFW_KEY_A) == GLFW_PRESS) {
    std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(-1.f, 0.f, 0.f));
    ZEngine::EventAgent()->TriggerEvent(moveEvent);
  }
  if (glfwGetKey(windowHandle, GLFW_KEY_S) == GLFW_PRESS) {
    std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(0.f, 0.f, -1.f));
    ZEngine::EventAgent()->TriggerEvent(moveEvent);
  }
  if (glfwGetKey(windowHandle, GLFW_KEY_D) == GLFW_PRESS) {
    std::shared_ptr<ZObjectMoveEvent> moveEvent(new ZObjectMoveEvent(1.f, 0.f, 0.f));
    ZEngine::EventAgent()->TriggerEvent(moveEvent);
  }
  if (glfwGetKey(windowHandle, GLFW_KEY_SPACE) == GLFW_PRESS)
  ;
  if (glfwGetKey(windowHandle, GLFW_KEY_TAB) == GLFW_PRESS)
  ;
  if (glfwGetKey(windowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    std::shared_ptr<ZQuitEvent> quitEvent(new ZQuitEvent);
    ZEngine::EventAgent()->TriggerEvent(quitEvent);
  }

  double yaw, pitch;
  glfwGetCursorPos(windowHandle, &yaw, &pitch);

  if (firstLook_) {
    lastYaw_ = yaw; lastPitch_ = pitch;
    firstLook_ = false;
  }
  if (yaw != lastYaw_) {
    std::shared_ptr<ZObjectLookEvent> lookEvent(new ZObjectLookEvent(yaw - lastYaw_, 0.f));
    ZEngine::EventAgent()->TriggerEvent(lookEvent);
  }
  if (pitch != lastPitch_) {
    std::shared_ptr<ZObjectLookEvent> lookEvent(new ZObjectLookEvent(0.f, lastPitch_ - pitch));
    ZEngine::EventAgent()->TriggerEvent(lookEvent);
  }
  lastYaw_ = yaw; lastPitch_ = pitch;

  if (glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePressState_[GLFW_MOUSE_BUTTON_LEFT]) {
    std::shared_ptr<ZFireEvent> fireEvent(new ZFireEvent);
    ZEngine::EventAgent()->TriggerEvent(fireEvent);
    mousePressState_[GLFW_MOUSE_BUTTON_LEFT] = true;
  } else if (glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
    mousePressState_[GLFW_MOUSE_BUTTON_LEFT] = false;
  }
}

void ZGLInput::GetPointerPosition(double& x, double& y) {
  GLFWwindow* windowHandle = glfwGetCurrentContext();
  glfwGetCursorPos(windowHandle, &x, &y);
}
