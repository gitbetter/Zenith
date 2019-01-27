//
//  ZGraphicsComponent.cpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#include "ZGraphicsComponent.hpp"
#include "ZModel.hpp"

void ZGraphicsComponent::Update(float frameMix) {
  // TODO: Use frameMix to interpolate between frames
  model_->Render(shader_);
}
