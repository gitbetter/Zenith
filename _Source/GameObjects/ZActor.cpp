//
//  ZActor.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGame.hpp"
#include "ZActor.hpp"
#include "ZCamera.hpp"
#include "ZGraphicsComponent.hpp"

void ZActor::Render(float frameMix) {
  ZGameObject::Render(frameMix);
  ZCamera* camera = game_->GetActiveCamera();
  graphicsComponent_->Update(camera, frameMix);
}

void ZActor::SetGraphicsComponent(ZGraphicsComponent* graphicsComponent) {
  graphicsComponent_ = graphicsComponent;
  graphicsComponent_->Translate(position_);
}
