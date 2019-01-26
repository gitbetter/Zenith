//
//  ZGraphicsComponent.cpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#include "ZGraphicsComponent.hpp"

ZGraphicsComponent::ZGraphicsComponent()  {}
ZGraphicsComponent::~ZGraphicsComponent() {}

void ZGraphicsComponent::Update(float frameMix) {
  // TODO: Use frameMix to interpolate between frames
  model->Render();
}
