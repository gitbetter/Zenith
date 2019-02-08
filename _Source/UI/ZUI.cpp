//
//  ZUI.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZUI.hpp"

void ZUI::Initialize() {
  if (graphicsStrategy_ == nullptr) {
    graphicsStrategy_ = new ZGraphicsStrategy();
    graphicsStrategy_->Initialize();
  }
}

void ZUI::Draw() {
  
}
