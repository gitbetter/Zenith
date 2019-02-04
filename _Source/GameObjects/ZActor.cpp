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
  ZGraphicsComponent* graphicsComp = FindComponent<ZGraphicsComponent>();
  if (graphicsComp != nullptr) {
    ZCamera* camera = game_->GetActiveCamera();
    graphicsComp->Update(camera, frameMix);
  }
}

void ZActor::ShouldTranslateWithView(bool translates) {
  ZGameObject::ShouldTranslateWithView(translates);
  ZGraphicsComponent* graphicsComp = FindComponent<ZGraphicsComponent>();
  if (graphicsComp != nullptr) {
    graphicsComp->ShouldTranslateWithView(translates);
  }
}
