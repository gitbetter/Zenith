//
//  ZGLPanel.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIPanel.hpp"
#include <string>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZGLPanel : public ZUIPanel {
private:
public:
    ZGLPanel(glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(1.f))
    : ZUIPanel(position, scale) { }
    ~ZGLPanel() { }

    void Render() override { }
protected:

};
