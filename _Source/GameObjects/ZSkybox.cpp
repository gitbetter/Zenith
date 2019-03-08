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
  std::shared_ptr<ZModel> skybox = ZModel::NewSkybox(hdrMap, iblTexture_);

  std::shared_ptr<ZShader> skyboxShader(new ZShader);
  skyboxShader->Initialize("Assets/Shaders/Vertex/skybox.vert", "Assets/Shaders/Pixel/skybox.frag");

  std::shared_ptr<ZGraphicsComponent> skyboxGraphicsComponent(new ZGraphicsComponent);
  skyboxGraphicsComponent->Initialize(skybox, skyboxShader);

  std::vector<ZTexture> textures = { iblTexture_.cubeMap };
  skyboxGraphicsComponent->AddMaterial(std::make_shared<ZMaterial>(textures));

  AddComponent(skyboxGraphicsComponent);
}
