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
public:
    static void RunGameLoop();

private:
    static bool running_;
    static std::vector<ZGameObject*> gameObjects_;
    static const float MS_PER_UPDATE;
    static const int MAX_FIXED_UPDATE_ITERATIONS;

    static void Update();
    static void Render(float frameMix);
};
