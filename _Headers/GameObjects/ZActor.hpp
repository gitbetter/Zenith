//
//  ZActor.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <glm/glm.hpp>
#include "ZGameObject.hpp"

// Forward Declarations
class ZGraphicsComponent;

// Class and Data Structure Definitions
class ZActor : public ZGameObject {
private:
  ZGraphicsComponent* graphicsComponent_ = nullptr;

public:
  ZActor(glm::vec3 position = glm::vec3(0.f)) : ZGameObject(position) { }
  ~ZActor() { }

  virtual void Update() override { };
  virtual void Render(float frameMix) override;

  // TODO: Move setter to ZGameObject and make it a templated method to store any type if component
  // Use a std::map to store typeid(T).name as the key and ZComponent* as the value, where T is the
  // templated parameter.
  ZGraphicsComponent* GetGraphicsComponent() const { return graphicsComponent_; }
  void SetGraphicsComponent(ZGraphicsComponent* graphicsComponent);

  virtual void ShouldTranslateWithView(bool translates) override;
protected:

};
