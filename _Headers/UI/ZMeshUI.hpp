//
//  ZMeshUI.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZMesh.hpp"

// Forward Declarations
// class SomeClass;

struct ZUIVertex {
  glm::vec3 position;
  glm::vec2 uv;
};

// Class and Data Structure Definitions
class ZMeshUI : ZMesh {
private:
public:
    ZMeshUI(std::vector<ZVertex> vertices) : ZMesh(vertices) { }
    virtual ~ZMeshUI() { }

    void Setup() { }
protected:

};
