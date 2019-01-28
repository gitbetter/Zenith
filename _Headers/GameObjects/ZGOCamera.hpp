//
//  ZGOCamera.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGameObject.hpp"
#include <iostream>

// Forward Declarations
class ZInputComponent;

// Class and Data Structure Definitions
class ZGOCamera : ZGameObject {
private:
public:
    ZGOCamera(ZInputComponent* inputComponent);
    ~ZGOCamera() { }

    virtual void Update() override;
    virtual void Render(float frameMix) override { };
protected:

};
