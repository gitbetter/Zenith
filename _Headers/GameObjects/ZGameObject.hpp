/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGameObject.hpp

    Created by Adrian Sanchez on 1/25/19.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

// Includes
#include "ZEngine.hpp"
#include "ZProcess.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZProcessRunner.hpp"

// Forward Declarations
class ZGame;
class ZGOFactory;
struct ZOFNode;

// Class Definitions
class ZGameObject : public ZProcess {
  friend class ZGame;
  friend class ZGOFactory;

private:

  void CalculateTangentBasis();

public:

  ZGameObject(glm::vec3 position = glm::vec3(0.f, 1.f, 0.f), glm::quat orientation = glm::quat(glm::vec3(0.f)));
  virtual ~ZGameObject() { }

  void Initialize(std::shared_ptr<ZOFNode> root);

  virtual void Render(float frameMix, RENDER_OP renderOp = RENDER_OP_COLOR) override;

  void CalculateDerivedData();

  void SetPosition(glm::vec3 position);
  void SetScale(glm::vec3 scale);
  void SetOrientation(glm::quat quaternion);
  void SetOrientation(glm::vec3 euler);
  void SetModelMatrix(glm::mat4 modelMatrix);

  glm::vec3 Position() const { return glm::vec3(position_); }
  glm::vec3 Scale() const { return scale_; }
  glm::quat Orientation() const { return orientation_; }
  glm::vec3 Front() const { return glm::conjugate(orientation_) * glm::vec3(0.f, 0.f, -1.f); }
  glm::vec3 Up() const { return glm::conjugate(orientation_) * glm::vec3(0.f, 1.f, 0.f); }
  glm::vec3 Right() const { return glm::conjugate(orientation_) * glm::vec3(-1.f, 0.f, 0.f); }
  glm::mat4 ModelMatrix(float frameMix = 0.f);

  glm::vec3 PreviousPosition() const { return glm::vec3(previousPosition_); }
  glm::vec3 PreviousFront() const { return glm::conjugate(previousOrientation_) * glm::vec3(0.f, 0.f, -1.f); }
  glm::vec3 PreviousUp() const { return glm::conjugate(previousOrientation_) * glm::vec3(0.f, 1.f, 0.f); }
  glm::vec3 PreviousRight() const { return glm::conjugate(previousOrientation_) * glm::vec3(-1.f, 0.f, 0.f); }

  ZGame* Game() const { return game_; }

  template<class T>
  typename std::enable_if<std::is_base_of<ZComponent, T>::value>::type
  AddComponent(std::shared_ptr<T> component) {
    std::shared_ptr<T> foundComponent = FindComponent<T>();
    if (foundComponent == nullptr) {
      component->object_ = this;
      components_.push_back(component);
      ZEngine::ProcessRunner()->AttachProcess(component);
    }
  }

  template<class T> std::shared_ptr<T> RemoveComponent() {
    std::vector<std::shared_ptr<ZComponent>>::iterator found;
    for (std::vector<std::shared_ptr<ZComponent>>::iterator it = components_.begin(); it != components_.end(); ++it) {
      if (std::dynamic_pointer_cast<T>(*it)) {
        found = it; break;
      }
    }

    if (found == components_.end()) return nullptr;

    std::shared_ptr<T> removed = *found;
    components_.erase(found);
    return removed;
  }

  template<class T> std::shared_ptr<T> FindComponent() {
    for (std::shared_ptr<ZComponent> comp : components_) {
      if (std::dynamic_pointer_cast<T>(comp)) return std::dynamic_pointer_cast<T>(comp);
    }
    return nullptr;
  }

protected:

  ZGame* game_ = nullptr;
  std::vector<std::shared_ptr<ZComponent>> components_;
  glm::vec4 position_, previousPosition_;
  glm::vec3 scale_, previousScale_;
  glm::quat orientation_, previousOrientation_;
  glm::mat4 modelMatrix_;
};
