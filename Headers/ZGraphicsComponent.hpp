//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

// Includes
#include <iostream>

// Forward Declarations
class ZModel;

// Class and Data Structure Definitions
class ZGraphicsComponent {
private:

public:
    ZGraphicsComponent() { };
    ~ZGraphicsComponent() { };

    virtual void Update(float frameMix);
    
protected:
    ZModel* model_ = nullptr;
};
