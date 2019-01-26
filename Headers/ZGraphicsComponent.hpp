//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

#include <iostream>

class ZGraphicsComponent {
private:
public:
    ZGraphicsComponent();
    ~ZGraphicsComponent();

    virtual void Update(float frameMix);
protected:
    ZModel* model_ = nullptr;
};
