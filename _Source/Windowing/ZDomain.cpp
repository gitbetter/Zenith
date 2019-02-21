//
//  ZWindow.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZDomain.hpp"
#include "ZGLDomainStrategy.hpp"

void ZDomain::Initialize() {
  // TODO: Switch the strategy here based on domain (windowing and input handling) implementation
  if (domainStrategy_ == nullptr) {
    domainStrategy_ = new ZGLDomainStrategy();
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
    delete domainStrategy_;
  }
}
