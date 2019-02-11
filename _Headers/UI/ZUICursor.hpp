//
//  ZUICursor.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 09/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIElement.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZUICursor : public ZUIElement {
private:
public:
    ZUICursor(glm::vec2 position = glm::vec2(0.5f), glm::vec2 scale = glm::vec2(0.02f, 0.03f));
    ~ZUICursor() { }

    void Render(ZShader* shader) override;

    void HandlePitch(float controlThrow) override;
    void HandleYaw(float controlThrow) override;
    void HandleFire() override;

    void SetCursorImage(std::string path);
    void SetColor(glm::vec4 color) override ;

protected:

};
