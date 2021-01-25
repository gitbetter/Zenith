/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZWindow.cpp

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

#include "ZDomain.hpp"
#include "ZGLDomainStrategy.hpp"
#include "ZWindowResizeEvent.hpp"
#include "ZEventAgent.hpp"

ZDomain::ZDomain(unsigned int windowWidth, unsigned int windowHeight)
{
    options_.windowSize.x = windowWidth;
    options_.windowSize.y = windowHeight;
}

void ZDomain::Initialize()
{
    if (domainStrategy_ == nullptr)
    {
        domainStrategy_.reset(new ZGLDomainStrategy);
        domainStrategy_->Initialize();
        window_ = domainStrategy_->CreateWindow(options_.windowSize.x, options_.windowSize.y, options_.maximized, options_.visible);
        if (!window_) {
            zenith::Log("Could not create glfw window", ZSeverity::Error);
            return;
        }

        domainStrategy_->OnFramebufferResized([this](int width, int height) {
            this->options_.resolution.x = width; this->options_.resolution.y = height;
        });

        domainStrategy_->OnWindowResized([this](int width, int height) {
            std::shared_ptr<ZWindowResizeEvent> windowResizeEvent = std::make_shared<ZWindowResizeEvent>();
            zenith::EventAgent()->QueueEvent(windowResizeEvent);
        });

        glm::vec2 frameBufferSize = domainStrategy_->FramebufferSize();
        options_.resolution.x = frameBufferSize.x; options_.resolution.y = frameBufferSize.y;
    }
}

void ZDomain::PollEvents()
{
    if (domainStrategy_) {
        domainStrategy_->PollEvents();
    }
}

void ZDomain::CaptureCursor()
{
    if (domainStrategy_) {
        domainStrategy_->CaptureCursor();
    }
}

void ZDomain::ReleaseCursor()
{
    if (domainStrategy_) {
        domainStrategy_->ReleaseCursor();
    }
}

void ZDomain::HideCursor()
{
    if (domainStrategy_) {
        domainStrategy_->HideCursor();
    }
}

void ZDomain::ShowCursor()
{
    if (domainStrategy_) {
        domainStrategy_->ShowCursor();
    }
}

bool ZDomain::IsCursorCaptured()
{
    if (domainStrategy_) {
        return domainStrategy_->IsCursorCaptured();
    }
}

bool ZDomain::IsCursorHidden()
{
    if (domainStrategy_) {
        return domainStrategy_->IsCursorHidden();
    }
    return true;
}

glm::vec2 ZDomain::FramebufferSize()
{
    if (domainStrategy_) {
        return domainStrategy_->FramebufferSize();
    }
    return glm::vec2(0.f);
}

void ZDomain::CloseWindow()
{
    if (domainStrategy_) {
        domainStrategy_->CloseWindow();
    }
}

bool ZDomain::IsWindowClosing()
{
    if (domainStrategy_) {
        return domainStrategy_->IsWindowClosing();
    }
    return false;
}

void ZDomain::SetWindowSize(int width, int height)
{
    if (domainStrategy_) {
        domainStrategy_->ResizeWindow(width, height);
    }
    options_.windowSize.x = width; options_.windowSize.y = height;
}

void ZDomain::SetResolution(int x, int y)
{
    if (domainStrategy_) {
        domainStrategy_->ResizeFramebuffer(x, y);
    }
    options_.resolution.x = x; options_.resolution.y = y;
}

void ZDomain::SetWindow(void* window)
{
    if (domainStrategy_) {
        window_ = window;
        domainStrategy_->SetWindow(window);
    }
}

void ZDomain::DestroyWindow(void* window)
{
    if (domainStrategy_) {
        domainStrategy_->DestroyWindow(window);
        window_ = nullptr;
    }
}

void ZDomain::CleanUp()
{
    if (domainStrategy_ != nullptr)
    {
        domainStrategy_->CleanUp();
        domainStrategy_.reset();
        window_ = nullptr;
    }
}
