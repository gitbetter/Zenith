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

// Forward Declarations
class ZGame;
class ZGOFactory;
struct ZOFNode;

// Class Definitions
class ZGameObject : public ZObject {
  friend class ZGame;
  friend class ZGOFactory;

private:
  void CalculateTangentBasis();

public:
  ZGameObject(glm::vec3 position = glm::vec3(0.f, 1.f, 0.f), glm::quat orientation = glm::quat(glm::vec3(0.f)));
  virtual ~ZGameObject() { }

  void Initialize(ZOFNode* root);

  virtual void Update();
  virtual void Render(float frameMix, unsigned char renderOp = ZGraphics::RENDER_OP_COLOR) { }

  void ShouldTranslateWithView(bool translates);
  void CalculateModelMatrix();

  void SetPosition(glm::vec3 position);
  void SetScale(glm::vec3 scale);
  void SetOrientation(glm::quat quaternion);
  void SetOrientation(glm::vec3 euler);

  glm::vec3 Position() const { return glm::vec3(position_); }
  glm::vec3 Scale() const { return scale_; }
  glm::quat Orientation() const { return orientation_; }
  glm::vec3 Front() const { return glm::conjugate(orientation_) * glm::vec3(0.f, 0.f, -1.f); }
  glm::vec3 Up() const { return glm::conjugate(orientation_) * glm::vec3(0.f, 1.f, 0.f); }
  glm::vec3 Right() const { return glm::conjugate(orientation_) * glm::vec3(-1.f, 0.f, 0.f); }
  glm::mat4 ModelMatrix() const { return modelMatrix_; }

  glm::vec3 PreviousFront() const { return glm::conjugate(previousOrientation_) * glm::vec3(0.f, 0.f, -1.f); }
  glm::vec3 PreviousUp() const { return glm::conjugate(previousOrientation_) * glm::vec3(0.f, 1.f, 0.f); }
  glm::vec3 PreviousRight() const { return glm::conjugate(previousOrientation_) * glm::vec3(-1.f, 0.f, 0.f); }

  template<class T>
  typename std::enable_if<std::is_base_of<ZComponent, T>::value>::type
  AddComponent(T* component) {
    T* foundComponent = FindComponent<T>();
    if (foundComponent != nullptr) {
      // TODO: Replace or ignore
    } else {
      component->object_ = this;
      components_.push_back(component);
    }
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

  template<class T> T* FindComponent() {
    for (ZComponent* comp : components_) {
      if (dynamic_cast<T*>(comp)) return dynamic_cast<T*>(comp);
    }
    return nullptr;
  }

protected:
  ZGame* game_ = nullptr;
  std::vector<ZComponent*> components_;
  glm::vec4 position_, previousPosition_;
  glm::vec3 scale_, previousScale_;
  glm::quat orientation_, previousOrientation_;
  glm::mat4 modelMatrix_;
  bool translatesWithView_;
};
