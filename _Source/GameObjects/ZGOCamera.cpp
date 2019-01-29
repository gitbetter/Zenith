//
//  ZGOCamera.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGOCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 ZGOCamera::GetViewMatrix() {
  return glm::lookAt(position_, position_ + front_, up_);
}
