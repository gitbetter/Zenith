//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGameObject.hpp"

void ZGameObject::setWorldPosition(glm::vec4 position){
    worldPosition_ = position;
}

const glm::vec4& ZGameObject::getWorldPosition() {
  return worldPosition_;
}
