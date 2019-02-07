//
//  ZGLButton.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIButton.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZGLButton : public ZUIButton {
private:
public:
    ZGLButton(glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(1.f))
    : ZUIButton(position, scale) { }
    ~ZGLButton() { }

    void Render() override { }
protected:

};
