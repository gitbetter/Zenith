/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZGLDomainStrategy.cpp

	Created by Adrian Sanchez on 07/02/2019.
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

#include "ZGLDomainStrategy.hpp"
#include "ZCommon.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void ZGLDomainStrategy::Initialize() {
	glfwSetErrorCallback(GLFWErrorCallback);
	glfwInit();	
}

void* ZGLDomainStrategy::CreateWindow(int width, int height, bool visible, void* sharedContext) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_VISIBLE, visible ? GL_TRUE : GL_FALSE);

	GLFWwindow* window;
	if (sharedContext == nullptr) {
		window = glfwCreateWindow(width, height, "Zenith", NULL, NULL);
		glfwMakeContextCurrent(window);
	} else {
		GLFWwindow* shared = static_cast<GLFWwindow*>(sharedContext);
		window = glfwCreateWindow(width, height, "Zenith", NULL, shared);
	}

	if (window == NULL) {
		_Z("Could not create glfw window", ZERROR);
		glfwTerminate();
	}

	if (visible) {
		glewExperimental = GL_TRUE;
		glewInit();

		glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

#ifndef EDITOR
		CaptureCursor();
#endif
	}
	return window;
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

glm::vec2 ZGLDomainStrategy::FramebufferSize() {
	GLFWwindow* glWindow = glfwGetCurrentContext();
	int width, height;
	glfwGetFramebufferSize(glWindow, &width, &height);
	return glm::vec2(width, height);
}

void ZGLDomainStrategy::CloseWindow() {
	GLFWwindow* glWindow = glfwGetCurrentContext();
	glfwSetWindowShouldClose(glWindow, GL_TRUE);
}

bool ZGLDomainStrategy::IsWindowClosing() {
	GLFWwindow* glWindow = glfwGetCurrentContext();
	return glfwWindowShouldClose(glWindow);
}

void* ZGLDomainStrategy::Context() {
	return glfwGetCurrentContext();
}

void ZGLDomainStrategy::SetContext(void* context) {
	GLFWwindow* glWindow = static_cast<GLFWwindow*>(context);
	glfwMakeContextCurrent(glWindow);
}

void ZGLDomainStrategy::DestroyContext(void* context) {
	GLFWwindow* glWindow = static_cast<GLFWwindow*>(context);
	if (glWindow) glfwDestroyWindow(glWindow);
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
