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
    domainStrategy_->CreateWindow(width_, height_);
    // Make sure the window size is in pixels and not screen coordinates,
    // which is done by querying the current framebuffer size
    glm::vec2 fbSize = domainStrategy_->FramebufferSize();
    width_ = fbSize.x; height_ = fbSize.y;
  }
}

void ZDomain::ResizeWindow(int width, int height) {
  if (domainStrategy_ != nullptr) {
    domainStrategy_->Resize(width, height);
  }
  width_ = width; height_ = height;
}

void ZDomain::CleanUp() {
  if (domainStrategy_ != nullptr) {
    domainStrategy_->CleanUp();
    delete domainStrategy_;
  }
}
