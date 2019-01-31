//
//  ZGLInput.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGLInput.hpp"
#include "ZGLWindow.hpp"
#include "ZIStrafeCommand.hpp"
#include "ZIForwardBackCommand.hpp"
#include "ZIEscapeCommand.hpp"
#include "ZIPitchCommand.hpp"
#include "ZIYawCommand.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cassert>

void ZGLInput::ProcessInput() {
  // Get the window and make sure it is a ZGLWindow (since this is ZGLInput)
  const ZGLWindow* glWindow = dynamic_cast<const ZGLWindow*>(ZEngine::GetGraphics()->GetWindow());
  assert(glWindow);

  GLFWwindow* windowHandle = glWindow->GetHandle();
  // Todo: Flyweight the commands
  if (glfwGetKey(windowHandle, GLFW_KEY_W) == GLFW_PRESS) Broadcast(ZIForwardBackCommand(1.0));
  if (glfwGetKey(windowHandle, GLFW_KEY_A) == GLFW_PRESS) Broadcast(ZIStrafeCommand(-1.0));
  if (glfwGetKey(windowHandle, GLFW_KEY_S) == GLFW_PRESS) Broadcast(ZIForwardBackCommand(-1.0));
  if (glfwGetKey(windowHandle, GLFW_KEY_D) == GLFW_PRESS) Broadcast(ZIStrafeCommand(1.0));
  if (glfwGetKey(windowHandle, GLFW_KEY_SPACE) == GLFW_PRESS)
  ;
  if (glfwGetKey(windowHandle, GLFW_KEY_TAB) == GLFW_PRESS)
  ;
  if (glfwGetKey(windowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS) Broadcast(ZIEscapeCommand());

  double yaw, pitch;
  glfwGetCursorPos(windowHandle, &yaw, &pitch);

  if (firstLook_) {
    lastYaw_ = yaw; lastPitch_ = pitch;
    firstLook_ = false;
  }
  if (yaw != lastYaw_) Broadcast(ZIYawCommand(yaw - lastYaw_));
  if (pitch != lastPitch_) Broadcast(ZIPitchCommand(lastPitch_ - pitch));
  lastYaw_ = yaw; lastPitch_ = pitch;
}
