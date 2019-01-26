//
//  TModel.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <glm/vec4.hpp>
#include <vector>

// Forward Declarations
class ZMesh;

// Class Definitions
class ZModel {
public:
    virtual void Render();
    
private:
    std::vector<ZMesh> meshes_;
};
