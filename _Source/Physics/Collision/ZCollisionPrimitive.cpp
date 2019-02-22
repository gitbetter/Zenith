//
//  ZCollisionPrimitive.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 21/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZCollisionPrimitive.hpp"

void ZCollisionPrimitive::CalculateInternals() {
  transform = body->ModelMatrix() * offset;
}
