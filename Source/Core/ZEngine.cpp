//
//  ZEngine.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZNullGraphics.hpp"

const ZEngine::float MS_PER_UPDATE = 17.0f;
const ZEngine::int MAX_FIXED_UPDATE_ITERATIONS = 50;
ZEngine::graphics_ = new ZNullGraphics();

ZGraphics* ZEngine::GetGraphics() {
  return graphics_;
}

static void ZEngine::Provide(ZGraphics& graphics) {
  graphics_ = graphics;
}
