//
//  ZActor.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZGame.hpp"
#include "ZActor.hpp"
#include "ZGraphicsComponent.hpp"

void ZActor::Render(float frameMix, unsigned char renderOp) {
  ZGameObject::Render(frameMix);
  ZGraphicsComponent* graphicsComp = FindComponent<ZGraphicsComponent>();
  if (graphicsComp != nullptr) {
    ZGameObject* camera = game_->GetActiveCamera();
    const std::map<std::string, ZLight*>& gameLights = game_->GetGameLights();
    graphicsComp->Update(gameLights, camera, frameMix, renderOp);
  }
}

void ZActor::ShouldTranslateWithView(bool translates) {
  ZGameObject::ShouldTranslateWithView(translates);
  ZGraphicsComponent* graphicsComp = FindComponent<ZGraphicsComponent>();
  if (graphicsComp != nullptr) {
    graphicsComp->ShouldTranslateWithView(translates);
  }
}
