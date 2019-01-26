//
//  ZModel.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZModel.hpp"
#include "ZMesh.hpp"

void ZModel::Render() {
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].Render();
    }
}
