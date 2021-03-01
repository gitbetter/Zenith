/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZDomain.hpp

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
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZDomain
{

public:

    ZDomain(const ZDomainOptions& options) : options_(options) {}
    virtual ~ZDomain() {}

    void* Window() const { return window_; }    
    glm::vec2 WindowSize() const { return options_.windowSize; }
    glm::vec2 Resolution() const { return options_.resolution; }
    double Aspect() const { return options_.resolution.y > 0 ? options_.resolution.x / options_.resolution.y : 0; }
    bool Visible() const { return !options_.offline; }
    bool Maximized() const { return options_.maximized; }

    virtual glm::vec2 FramebufferSize() = 0;

    virtual void Initialize() = 0;
    virtual void SwapBuffers() = 0;
    virtual void ResizeWindow(int width, int height) = 0;
    virtual void ResizeFramebuffer(int width, int height) = 0;
    virtual void SetSizeLimits(const glm::vec2& min, const glm::vec2& max) = 0;
    virtual bool IsClosing() = 0;
    virtual void Close() = 0;
    virtual void SetWindow(void* window) = 0;
    virtual void SetAsCurrent() = 0;
    virtual void Destroy() = 0;
    virtual void SetCursor(const ZCursor& cursor) = 0;
    virtual void OnWindowResized(const std::function<void(int, int)>& callback) = 0;
    virtual void OnFramebufferResized(const std::function<void(int, int)>& callback) = 0;
    virtual void CleanUp() = 0;

    static std::shared_ptr<ZDomain> Create(const ZDomainOptions& options);

protected:

    void* window_ = nullptr;
    std::vector<ZCursor> cursors_;
    ZDomainOptions options_;

    virtual void CreateWindow(int width, int height, bool maximized = true, bool visible = true, void* sharedContext = nullptr) = 0;

};
