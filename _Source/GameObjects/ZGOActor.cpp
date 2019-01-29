//
//  ZGOActor.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGOActor.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZGameObject.hpp"

void ZGOActor::Render(float frameMix) {
  ZGameObject::Render(frameMix);
  graphicsComponent_->Update(frameMix);
}
