/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZSkybox.cpp

    Created by Adrian Sanchez on 02/03/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

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
  ZEngine::ProcessRunner()->AttachProcess(skyboxGraphicsComponent);
  properties_.renderPass = ZRenderPass::Sky;
}
