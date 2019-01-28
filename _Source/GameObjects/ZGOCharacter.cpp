//
//  ZGOCharacter.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGOCharacter.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZGameObject.hpp"

void ZGOCharacter::Render(float frameMix) {
  ZGameObject::Render(frameMix);
  graphicsComponent_->Update(frameMix);
}
