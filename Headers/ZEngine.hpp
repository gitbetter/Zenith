//
//  ZEngine.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>

// Forward Declarations
class ZGameObject;

// Class Definitions
class ZEngine {
private:
    static const float MS_PER_UPDATE;
    static const int MAX_FIXED_UPDATE_ITERATIONS;
    static bool running_;
    static std::vector<ZGameObject*> gameObjects_;

    static void Update();
    static void Render(float frameMix);
    
public:
    static void RunGameLoop();
};
