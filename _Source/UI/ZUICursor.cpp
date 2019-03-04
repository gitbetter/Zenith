//
//  ZUICursor.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
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
  ZUIImage* cursorImage = new ZUIImage("Assets/Textures/z_cursor.png", position, scale);
  AddChild(cursorImage);
}

void ZUICursor::Draw(ZShader* shader) {
  ZUIElement::Draw(shader);
  RenderChildren(shader);
}

void ZUICursor::SetCursorImage(std::string path) {
  for (ZUIElement* child : children_) {
    ZUIImage* uiImage = dynamic_cast<ZUIImage*>(child);
    if (uiImage != nullptr) {
      uiImage->SetImage(path);
    }
  }
}

void ZUICursor::SetColor(glm::vec4 color) {
  ZUIElement::SetColor(color);
  for (ZUIElement* child : children_) {
    ZUIImage* uiImage = dynamic_cast<ZUIImage*>(child);
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
  std::vector<ZUIElement*> elements = ZEngine::UI()->Elements();
  bool elementFired = false;
  for (ZUIElement* element : elements) {
    if (!element->Selectable()) continue;
    if (Position().x >= element->Position().x - element->Size().x && Position().x <= element->Position().x + element->Size().x &&
        Position().y >= element->Position().y - element->Size().y && Position().y <= element->Position().y + element->Size().y) {
          element->Fire(ZEventType::FirePress);
          elementFired = true; break;
    }
  }

  // If a UI element is selected, underlying gameobjects should not be picked
  // as well, to preserve Z ordering
  if (elementFired) return;

  ZGameObject* camera = ZEngine::Game()->ActiveCamera();
  if (camera) {
    ZCameraComponent* cameraComp = camera->FindComponent<ZCameraComponent>();
    glm::mat4 InverseProjection = glm::inverse(cameraComp->ProjectionMatrix());
    glm::mat4 InverseView = glm::inverse(cameraComp->ViewMatrix(1.f));

    glm::vec4 rayStart(Position().x * 2.f - 1.f, Position().y * 2.f - 1.f, -1.f, 1.f);
    glm::vec4 rayEnd(Position().x * 2.f - 1.f, Position().y * 2.f - 1.f, 0.f, 1.f);

    glm::vec4 rayStartCamera = InverseProjection * rayStart; rayStartCamera /= rayStartCamera.w;
    glm::vec4 rayStartWorld = InverseView * rayStartCamera; rayStartWorld /= rayStartWorld.w;
    glm::vec4 rayEndCamera = InverseProjection * rayEnd; rayEndCamera /= rayEndCamera.w;
    glm::vec4 rayEndWorld = InverseView * rayEndCamera; rayEndWorld /= rayEndWorld.w;

    glm::vec3 rayDir = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));

    ZGameObject* objectHit = ZEngine::Physics()->Raycast(rayStartWorld, rayDir);
    if (objectHit) {
      _Z("Hit an object!", ZINFO);
    } else {
      _Z("Missed...", ZINFO);
    }
  }
}
