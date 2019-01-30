//
//  ZModel.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZModelImporter.hpp"

ZModel::ZModel(std::string path) {
  ZModelImporter importer;
  importer.LoadModel(path, meshes_);
}

void ZModel::Render(ZShader* shader) {
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].Render(shader);
    }
}
