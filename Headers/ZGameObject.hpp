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
class ZGraphicsComponent;

// Class Definitions
class ZGameObject {
public:
    ZGameObject(ZGraphicsComponent* graphicsComponent);
    ~ZGameObject();
    virtual void Update();
    virtual void Render(float frameMix);

private:
    glm::vec4 _worldPosition_;
    ZGraphicsComponent* graphicsComponent_ = nullptr;
};
