//
//  ZGLImage.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIImage.hpp"
#include <string>
#include <glm/glm.hpp>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZGLImage : public ZUIImage {
private:
public:
    ZGLImage(std::string path, glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(1.f))
    : ZUIImage(position, scale) { }
    ~ZGLImage() { }

    void Render() override { }
protected:

};
