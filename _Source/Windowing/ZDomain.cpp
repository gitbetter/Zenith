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

void ZDomain::Initialize() {
  // TODO: Switch the strategy here based on domain (windowing and input handling) implementation
  if (domainStrategy_ == nullptr) {
    domainStrategy_.reset(new ZGLDomainStrategy);
    domainStrategy_->Initialize();
    domainStrategy_->CreateWindow(windowWidth_, windowHeight_);

    glm::vec2 fbSize = domainStrategy_->FramebufferSize();
    resolutionX_ = fbSize.x; resolutionY_ = fbSize.y;
  }
}

void ZDomain::ResizeWindow(int width, int height) {
  if (domainStrategy_ != nullptr) {
    domainStrategy_->Resize(width, height);
  }
  windowWidth_ = width; windowHeight_ = height;
}

void ZDomain::CleanUp() {
  if (domainStrategy_ != nullptr) {
    domainStrategy_->CleanUp();
    domainStrategy_.reset();
  }
}
