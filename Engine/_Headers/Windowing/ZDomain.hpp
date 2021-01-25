/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZDomain.hpp

    Created by Adrian Sanchez on 27/01/2019.
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

// Class and Data Structure Definitions
class ZDomain
{

public:

    ZDomain(const ZDomainOptions& options) : options_(options), window_(nullptr) {}
    ZDomain(unsigned int windowWidth, unsigned int windowHeight);
    virtual ~ZDomain() {}

    void Initialize();

    unsigned int WindowWidth() const { return options_.windowSize.x; }
    unsigned int WindowHeight() const { return options_.windowSize.y; }
    unsigned int ResolutionX() const { return options_.resolution.x; }
    unsigned int ResolutionY() const { return options_.resolution.y; }
    float ResolutionXRatio() const { return (float)options_.resolution.x / zenith::DEFAULT_X_RESOLUTION; }
    float ResolutionYRatio() const { return (float)options_.resolution.y / zenith::DEFAULT_Y_RESOLUTION; }
    float Aspect() const { return (float)options_.resolution.x / (float)options_.resolution.y; }

    void* Window() { return window_; }

    void PollEvents();
    void CaptureCursor();
    void ReleaseCursor();
    void HideCursor();
    void ShowCursor();
    bool IsCursorCaptured();
    bool IsCursorHidden();
    glm::vec2 FramebufferSize();
    void CloseWindow();
    bool IsWindowClosing();
    void SetWindowSize(int x, int y);
    void SetResolution(int x, int y);
    void SetWindow(void* window);
    void DestroyWindow(void* window);
    void CleanUp();

protected:

    std::unique_ptr<ZDomainStrategy> domainStrategy_;
    ZDomainOptions options_;
    void* window_;

};
