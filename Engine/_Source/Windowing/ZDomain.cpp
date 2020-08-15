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

ZDomain::ZDomain(unsigned int windowWidth, unsigned int windowHeight)
{
    options_.width = windowWidth;
    options_.height = windowHeight;
}

void ZDomain::Initialize()
{
    if (domainStrategy_ == nullptr)
    {
        domainStrategy_.reset(new ZGLDomainStrategy);
        domainStrategy_->Initialize();
        mainContext_ = domainStrategy_->CreateWindow(options_.width, options_.height, options_.maximized);

        glm::vec2 frameBufferSize = domainStrategy_->FramebufferSize();
        resolutionX_ = frameBufferSize.x; resolutionY_ = frameBufferSize.y;
    }
}

void ZDomain::ResizeWindow(int width, int height)
{
    if (domainStrategy_ != nullptr)
    {
        domainStrategy_->Resize(width, height);
    }
    options_.width = width; options_.height = height;
}

void ZDomain::SetResolution(unsigned int x, unsigned int y)
{
    resolutionX_ = x; resolutionY_ = y;
}

void ZDomain::CleanUp()
{
    if (domainStrategy_ != nullptr)
    {
        domainStrategy_->CleanUp();
        domainStrategy_.reset();
    }
}
