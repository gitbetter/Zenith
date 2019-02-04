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
public:
  ZActor(glm::vec3 position = glm::vec3(0.f)) : ZGameObject(position) { }
  ~ZActor() { }

  virtual void Update() override { };
  virtual void Render(float frameMix) override;

  virtual void ShouldTranslateWithView(bool translates) override;
protected:

};
