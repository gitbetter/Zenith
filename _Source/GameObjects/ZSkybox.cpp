//
//  ZSkybox.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 02/03/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZSkybox.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZGraphicsComponent.hpp"

void ZSkybox::Initialize(std::string hdrMap) {
  ZModel* skybox = ZModel::NewSkybox(hdrMap, iblTexture_);

  ZShader* skyboxShader = new ZShader;
  skyboxShader->Initialize("Assets/Shaders/Vertex/skybox.vert", "Assets/Shaders/Pixel/skybox.frag");
  ZGraphicsComponent* skyboxGraphicsComponent = new ZGraphicsComponent;
  skyboxGraphicsComponent->Initialize(skybox, skyboxShader);

  AddComponent(skyboxGraphicsComponent);
}
