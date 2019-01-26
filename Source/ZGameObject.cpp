//
//  ZGameObject.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGameObject.hpp"
#include "ZGraphicsComponent.hpp"

ZGameObject::ZGameObject(ZGraphicsComponent* graphicsComponent) {
  graphicsComponent_ = graphicsComponent;
}

ZGameObject::~ZGameObject() { }

void ZGameObject::Update() { }

void ZGameObject::Render(float frameMix) {
  graphicsComponent_->Update(frameMix);
}
