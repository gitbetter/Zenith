//
//  ZUICursor.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZCommon.hpp"
#include "ZEngine.hpp"
#include "ZUICursor.hpp"
#include "ZUIImage.hpp"
#include "ZDomain.hpp"
#include "ZUI.hpp"
#include "ZGame.hpp"
#include "ZPhysics.hpp"
#include "ZCameraComponent.hpp"

ZUICursor::ZUICursor(glm::vec2 position, glm::vec2 scale) : ZUIElement(position, scale) {
  auto cursorImage = std::make_shared<ZUIImage>("Assets/Textures/z_cursor.png", position, scale);
  AddChild(cursorImage);
}

void ZUICursor::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);
}

void ZUICursor::Draw(ZShader* shader) {
  ZUIElement::Draw(shader);
  RenderChildren(shader);
}

void ZUICursor::SetCursorImage(std::string path) {
  for (std::shared_ptr<ZUIElement> child : children_) {
    std::shared_ptr<ZUIImage> uiImage = std::dynamic_pointer_cast<ZUIImage>(child);
    if (uiImage != nullptr) {
      uiImage->SetImage(path);
    }
  }
}

void ZUICursor::SetColor(glm::vec4 color) {
  ZUIElement::SetColor(color);
  for (std::shared_ptr<ZUIElement> child : children_) {
    std::shared_ptr<ZUIImage> uiImage = std::dynamic_pointer_cast<ZUIImage>(child);
    if (uiImage != nullptr) {
      uiImage->SetColor(color);
    }
  }
}

void ZUICursor::HandlePitch(float controlThrow) {
  Translate(glm::vec2(0.f, -controlThrow * 0.05f * ZEngine::DeltaTime()));
}

void ZUICursor::HandleYaw(float controlThrow) {
  Translate(glm::vec2(controlThrow * 0.05f * ZEngine::DeltaTime(), 0.f));
}

void ZUICursor::HandleFire() {
  ZUIElementMap elements = ZEngine::UI()->Elements();
  bool elementFired = false;
  for (ZUIElementMap::iterator it = elements.begin(); it != elements.end(); it++) {
    if (!it->second->Enabled()) continue;
    if (Position().x >= it->second->Position().x - it->second->Size().x && Position().x <= it->second->Position().x + it->second->Size().x &&
        Position().y >= it->second->Position().y - it->second->Size().y && Position().y <= it->second->Position().y + it->second->Size().y) {
          it->second->Fire(ZEventType::FirePress);
          elementFired = true; break;
    }
  }

  // If a UI element is selected, underlying game objects should not be picked (i.e. Z ordering is preserved)
  if (elementFired) return;

  std::shared_ptr<ZGameObject> camera = ZEngine::Game()->ActiveCamera();
  if (camera) {
    std::shared_ptr<ZCameraComponent> cameraComp = camera->FindComponent<ZCameraComponent>();
    glm::mat4 InverseProjection = glm::inverse(cameraComp->ProjectionMatrix());
    glm::mat4 InverseView = glm::inverse(cameraComp->ViewMatrix(1.f));

    glm::vec4 rayStart(Position().x * 2.f - 1.f, -Position().y * 2.f + 1.f, -1.f, 1.f);
    glm::vec4 rayEnd(Position().x * 2.f - 1.f, -Position().y * 2.f + 1.f, 0.f, 1.f);

    glm::vec4 rayStartCamera = InverseProjection * rayStart; rayStartCamera /= rayStartCamera.w;
    glm::vec4 rayStartWorld = InverseView * rayStartCamera; rayStartWorld /= rayStartWorld.w;
    glm::vec4 rayEndCamera = InverseProjection * rayEnd; rayEndCamera /= rayEndCamera.w;
    glm::vec4 rayEndWorld = InverseView * rayEndCamera; rayEndWorld /= rayEndWorld.w;

    glm::vec3 rayDir = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));

    ZGameObject* objectHit = ZEngine::Physics()->Raycast(rayStartWorld, rayDir);
    if (objectHit) {
      _Z("Object " + objectHit->ID() + " hit", ZINFO);
    }
  }
}
