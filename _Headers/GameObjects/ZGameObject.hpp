//
//  TGameObject.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <glm/vec4.hpp>

// Forward Declarations

// Class Definitions
class ZGameObject {
private:
  glm::vec4 worldPosition_;

public:
    virtual void Update() { }
    virtual void Render(float frameMix) { }

    void setWorldPosition(glm::vec4 position);
    const glm::vec4& getWorldPosition();
};
