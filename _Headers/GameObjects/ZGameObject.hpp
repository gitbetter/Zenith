//
//  ZGameObject.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZEngine.hpp"
#include "ZObject.hpp"
#include "ZGraphicsComponent.hpp"
#include <glm/glm.hpp>
#include <type_traits>

// Forward Declarations
class ZGame;

// Class Definitions
class ZGameObject : public ZObject {
  friend class ZGame;

private:
  void CalculateTangentBasis();

public:
  ZGameObject(glm::vec3 position = glm::vec3(0.f, 1.f, 0.f), glm::vec3 rotation = glm::vec3(0.f, -90.f, 0.f))
  : position_(glm::vec4(position, 1.f)),
    eulerRotation_(glm::vec4(rotation, 1.f)),
    front_(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)),
    up_(ZEngine::WORLD_UP),
    translatesWithView_(false),
    eulerVelocity_(glm::vec4(0.f)),
    eulerDamping_(0.05f)
  { CalculateTangentBasis(); }
  virtual ~ZGameObject() { }

  virtual void Update() { }
  virtual void Render(float frameMix, unsigned char renderOp = ZGraphics::RENDER_OP_COLOR) { }

  void SetPosition(glm::vec3 position) { position_ = glm::vec4(position, 1.f); }
  void SetRotation(glm::vec3 rotation) { eulerRotation_ = glm::vec4(rotation, 0.f); }
  void SetFrontVector(glm::vec3 front);
  virtual void ShouldTranslateWithView(bool translates) { translatesWithView_ = translates; }

  glm::mat4 ViewMatrix(float frameMix);
  glm::vec3 Position() const { return glm::vec3(position_); }
  glm::vec3 Rotation() const { return glm::vec3(eulerRotation_); }
  glm::vec3 FrontVector() const { return glm::vec3(front_); }
  glm::vec3 UpVector() const { return glm::vec3(up_); }
  glm::vec3 RightVector() const { return glm::vec3(right_); }

  template<class T>
  typename std::enable_if<std::is_base_of<ZComponent, T>::value>::type
  AddComponent(T* component) {
    T* foundComponent = FindComponent<T>();
    if (foundComponent != nullptr) {
      // TODO: Replace or ignore
    } else {
      if (std::is_same<T, ZGraphicsComponent>::value)
        dynamic_cast<ZGraphicsComponent*>(component)->Translate(position_);
      component->object_ = this;
      components_.push_back(component);
    }
  }

  template<class T> T* FindComponent() {
    for (ZComponent* comp : components_) {
      if (dynamic_cast<T*>(comp)) return dynamic_cast<T*>(comp);
    }
    return nullptr;
  }

  template<class T> T* RemoveComponent() {
    std::vector<ZComponent*>::iterator found;
    for (std::vector<ZComponent*>::iterator it = components_.begin(); it != components_.end(); ++it) {
      if (dynamic_cast<T*>(*it)) {
        found = it; break;
      }
    }

    if (found == components_.end()) return nullptr;

    T* removed = *found;
    components_.erase(found);
    return removed;
  }

protected:
  glm::vec4 position_, eulerRotation_, front_, up_, right_;
  glm::vec4 previousPosition_, previousEuler_, previousFront_, previousUp_, previousRight_;
  bool translatesWithView_;
  ZGame* game_;
  std::vector<ZComponent*> components_;
  /////////////////////////
  // TODO: This might more appropriately belong in a physics or separate movement component
  glm::vec4 eulerVelocity_;
  float eulerDamping_;
  /////////////////////////

  void UpdateFrontVectorRotation();
};
