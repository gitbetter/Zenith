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
  right_ = glm::normalize(glm::cross(front_, ZEngine::WORLD_UP));
  up_ = glm::normalize(glm::cross(right_, front_));
}
