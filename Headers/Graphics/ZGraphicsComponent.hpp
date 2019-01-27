//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

// Includes

// Forward Declarations
class ZModel;
class ZShader;

// Class and Data Structure Definitions
class ZGraphicsComponent {
private:

public:
    ZGraphicsComponent() { }
    ~ZGraphicsComponent() { }

    virtual void Update(float frameMix);

protected:
    ZModel* model_ = nullptr;
    ZShader* shader_ = nullptr;
};
