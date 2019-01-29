//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGameObject.hpp"

void ZGameObject::SetFrontVector(glm::vec3 front) {
  front_ = glm::normalize(front);
  CalculateTangentBasis();
}

void ZGameObject::CalculateTangentBasis() {
  up_ = glm::normalize(glm::cross(ZEngine::WORLD_UP, front_));
  right_ = glm::normalize(glm::cross(up_, front_));
}
