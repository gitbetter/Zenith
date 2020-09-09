/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLDomainStrategy.hpp

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

#pragma once

// Includes
#include "ZEngine.hpp"
#include "ZDomainStrategy.hpp"

// Forward Declarations
struct GLFWwindow;

// Class and Data Structure Definitions
class ZGLDomainStrategy : public ZDomainStrategy
{

private:

    static void GLFWErrorCallback(int id, const char* description);
    static void FrameBufferSizeCallback(GLFWwindow* window, int height, int width);
    static void WindowSizeCallback(GLFWwindow* window, int height, int width);

public:

    ZGLDomainStrategy() {}
    ~ZGLDomainStrategy() {}

    void Initialize() override;
    void* CreateWindow(int width, int height, bool maximized = true, bool visible = true, void* sharedContext = nullptr) override;
    void PollEvents() override;
    void CaptureCursor() override;
    void ReleaseCursor() override;
    void HideCursor() override;
    void ShowCursor() override;
    bool IsCursorCaptured() override;
    bool IsCursorHidden() override;
    void Resize(int width, int height) override;
    glm::vec2 FramebufferSize() override;
    void CloseWindow() override;
    bool IsWindowClosing() override;
    void* Context() override;
    void SetContext(void* context) override;
    void DestroyContext(void* context) override;
    void CleanUp() override;

};
