/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLDomain.cpp

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

#include "ZGLDomain.hpp"
#include "ZServices.hpp"
#include "ZWindowResizeEvent.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void ZGLDomain::Initialize()
{
    CreateWindow(options_.windowSize.x, options_.windowSize.y, options_.maximized, !options_.offline);
    if (!window_) {
        LOG("Could not create glfw window", ZSeverity::Error);
        return;
    }

    ZServices::Input()->RefreshContext();

    OnFramebufferResized([this](int width, int height) {
        this->options_.resolution.x = width; this->options_.resolution.y = height;
    });

    OnWindowResized([this](int width, int height) {
        std::shared_ptr<ZWindowResizeEvent> windowResizeEvent = std::make_shared<ZWindowResizeEvent>();
        ZServices::EventAgent()->Queue(windowResizeEvent);
    });

    glm::vec2 frameBufferSize = FramebufferSize();
    options_.resolution.x = frameBufferSize.x;
    options_.resolution.y = frameBufferSize.y;

    glfwSetErrorCallback(GLFWErrorCallback);
}

void ZGLDomain::SwapBuffers()
{
    GLFWwindow* glWindow = static_cast<GLFWwindow*>(window_);
    glfwSwapBuffers(glWindow);
}

void ZGLDomain::CreateWindow(int width, int height, bool maximized, bool visible, void* sharedContext)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_VISIBLE, visible ? GL_TRUE : GL_FALSE);

    if (maximized)
    {
        glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    }

    GLFWwindow* window;
    if (sharedContext == nullptr)
    {
        window = glfwCreateWindow(width, height, "Zenith", NULL, NULL);
        if (visible) glfwMakeContextCurrent(window);
    }
    else
    {
        GLFWwindow* shared = static_cast<GLFWwindow*>(sharedContext);
        window = glfwCreateWindow(width, height, "Zenith", NULL, shared);
    }

    if (window == NULL)
    {
        glfwTerminate();
        return;
    }

    glewExperimental = GL_TRUE;
    glewInit();

    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetWindowUserPointer(window, this);

    if (visible)
    {
        glfwSetWindowSizeCallback(window, WindowSizeCallback);
    }

    window_ = window;
}

void ZGLDomain::ResizeWindow(int width, int height)
{
    GLFWwindow* glWindow = static_cast<GLFWwindow*>(window_);
    glfwSetWindowSize(glWindow, width, height);
}

glm::vec2 ZGLDomain::FramebufferSize()
{
    GLFWwindow* glWindow = static_cast<GLFWwindow*>(window_);
    int width, height;
    glfwGetFramebufferSize(glWindow, &width, &height);
    return glm::vec2(width, height);
}

void ZGLDomain::Close()
{
    GLFWwindow* glWindow = static_cast<GLFWwindow*>(window_);
    glfwSetWindowShouldClose(glWindow, GL_TRUE);
}

bool ZGLDomain::IsClosing()
{
    GLFWwindow* glWindow = static_cast<GLFWwindow*>(window_);
    return glfwWindowShouldClose(glWindow);
}

void ZGLDomain::SetWindow(void* window)
{
    window_ = window;
}

void ZGLDomain::SetAsCurrent()
{
    GLFWwindow* glWindow = static_cast<GLFWwindow*>(window_);
    glfwMakeContextCurrent(glWindow);
}

void ZGLDomain::Destroy()
{
    GLFWwindow* glWindow = static_cast<GLFWwindow*>(window_);
    if (glWindow) glfwDestroyWindow(glWindow);
    window_ = nullptr;
}

void ZGLDomain::OnWindowResized(const std::function<void(int, int)>& callback)
{
    windowResizedCallback_ = callback;
}

void ZGLDomain::OnFramebufferResized(const std::function<void(int, int)>& callback)
{
    framebufferResizedCallback_ = callback;
}

void ZGLDomain::CleanUp()
{
    Destroy();
}

void ZGLDomain::WindowResize(int width, int height)
{
    if (windowResizedCallback_) {
        windowResizedCallback_(width, height);
    }
}

void ZGLDomain::FramebufferResize(int width, int height)
{
    if (framebufferResizedCallback_) {
        framebufferResizedCallback_(width, height);
    }
}

void ZGLDomain::GLFWErrorCallback(int id, const char* description)
{
    LOG(description, ZSeverity::Error);
}

void ZGLDomain::FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    ZDomain* domainStrategyPtr = static_cast<ZDomain*>(glfwGetWindowUserPointer(window));
    if (domainStrategyPtr) {
        domainStrategyPtr->FramebufferResize(width, height);
    }
    glViewport(0, 0, width, height);
}

void ZGLDomain::WindowSizeCallback(GLFWwindow* window, int height, int width)
{
    ZDomain* domainStrategyPtr = static_cast<ZDomain*>(glfwGetWindowUserPointer(window));
    if (domainStrategyPtr) {
        domainStrategyPtr->WindowResize(width, height);
    }
}
