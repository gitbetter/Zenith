//
//  ZMesh3D.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZMesh3D.hpp"
#include "ZEngine.hpp"
#include "ZShader.hpp"
#include "ZCommon.hpp"

ZMesh3D::ZMesh3D(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices, ZMaterial material)
: vertices_(vertices), indices_(indices), material_(material) {
  bufferData_ = ZEngine::Graphics()->Strategy()->LoadVertexData(vertices, indices);
}

void ZMesh3D::Render(ZShader* shader) {
  shader->Activate();
  AttachMaterialTextures(shader);
  ZEngine::Graphics()->Strategy()->Draw(bufferData_, vertices_, indices_);
}

void ZMesh3D::AttachMaterialTextures(ZShader* shader) {
  shader->Use(material_);
  // Loop through as many textures as we have and bind
  // the corresponding textures (preloaded) and texture samplers (shader side uniforms)
  unsigned int diffuseMaps = 1, specularMaps = 1, normalMaps = 1, heightMaps = 1, cubeMaps = 1;
  for (unsigned int i = 0; i < material_.Textures().size(); i++) {
    std::string textureNumber, textureName = material_.Textures()[i].type;
    if (textureName == "diffuse") textureNumber = std::to_string(diffuseMaps++);
    else if (textureName == "specular") textureNumber = std::to_string(specularMaps++);
    else if (textureName == "normal") textureNumber = std::to_string(normalMaps++);
    else if (textureName == "height") textureNumber = std::to_string(heightMaps++);
    else if (textureName == "cubemap") textureNumber = std::to_string(cubeMaps++);

    // Set the texture sampler to the correct unit and bind the texture
    shader->SetInt(textureName + textureNumber, i+1);
    ZEngine::Graphics()->Strategy()->BindTexture(material_.Textures()[i], i+1);
  }
}
