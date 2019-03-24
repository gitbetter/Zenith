/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGLInput.cpp

    Created by Adrian Sanchez on 28/01/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGLInput.hpp"
#include "ZEventAgent.hpp"
#include "ZObjectLookEvent.hpp"
#include "ZObjectDragEvent.hpp"
#include "ZObjectMoveEvent.hpp"
#include "ZQuitEvent.hpp"
#include "ZFireEvent.hpp"
#include <cassert>

void ZGLInput::Update() {
  GLFWwindow* windowHandle = glfwGetCurrentContext();
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
    if (mousePress_[GLFW_MOUSE_BUTTON_LEFT]) {
      std::shared_ptr<ZObjectDragEvent> dragEvent(new ZObjectDragEvent(yaw - lastYaw_, 0.f, 0.f));
      ZEngine::EventAgent()->TriggerEvent(dragEvent);
    }
    std::shared_ptr<ZObjectLookEvent> lookEvent(new ZObjectLookEvent(yaw - lastYaw_, 0.f));
    ZEngine::EventAgent()->TriggerEvent(lookEvent);
  }
  if (pitch != lastPitch_) {
    if (mousePress_[GLFW_MOUSE_BUTTON_LEFT]) {
      std::shared_ptr<ZObjectDragEvent> dragEvent(new ZObjectDragEvent(0.f, lastPitch_ - pitch, 0.f));
      ZEngine::EventAgent()->TriggerEvent(dragEvent);
    }
    std::shared_ptr<ZObjectLookEvent> lookEvent(new ZObjectLookEvent(0.f, lastPitch_ - pitch));
    ZEngine::EventAgent()->TriggerEvent(lookEvent);
  }
  lastYaw_ = yaw; lastPitch_ = pitch;

  if (glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePress_[GLFW_MOUSE_BUTTON_LEFT]) {
    std::shared_ptr<ZFireEvent> fireEvent(new ZFireEvent(yaw, pitch, 0.f));
    ZEngine::EventAgent()->TriggerEvent(fireEvent);
    mousePress_[GLFW_MOUSE_BUTTON_LEFT] = true;
  } else if (glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
    mousePress_[GLFW_MOUSE_BUTTON_LEFT] = false;
  }
}

void ZGLInput::GetPointerPosition(double& x, double& y) {
  GLFWwindow* windowHandle = glfwGetCurrentContext();
  glfwGetCursorPos(windowHandle, &x, &y);
}
