//
//  TModel.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh.hpp"
#include <glm/vec4.hpp>
#include <vector>
#include <string>

// Forward Declarations
//class ZMesh;
class ZShader;

// Class Definitions
class ZModel {
private:
  std::vector<ZMesh> meshes_;

public:
    ZModel(std::string path);
    ~ZModel() { }

    virtual void Render(ZShader* shader);
};
