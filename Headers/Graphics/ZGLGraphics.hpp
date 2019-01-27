//
//  ZGLGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphics.hpp"
#include <vector>

// Forward Declarations
class ZGLWindow;
class ZGameObject;

// Class and Data Structure Definitions
class ZGLGraphics : public ZGraphics {
private:

public:
    ZGLGraphics(int windowWidth, int windowHeight);
    virtual ~ZGLGraphics();

    virtual void Draw(const std::vector<ZGameObject*>& gameObjects, float frameMix) override;
    virtual void Delete() override;
    virtual const ZWindow* GetWindow() override;
protected:

};
