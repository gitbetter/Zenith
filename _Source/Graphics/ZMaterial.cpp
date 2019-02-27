//
//  ZMaterial.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 01/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZMaterial.hpp"

ZMaterial ZMaterial::DefaultMaterialSimple() {
  ZMaterialProperties materialProperties;
  materialProperties.albedo = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
  materialProperties.emission = 0.f;
  materialProperties.diffuse = 0.5f;
  materialProperties.ambient = 0.3f;
  materialProperties.specular = 0.2f;
  materialProperties.shininess = 0.2f;
  ZMaterial material(materialProperties);
  material.SetPBR(false);
  return material;
}

ZMaterial ZMaterial::DefaultMaterialPBR() {
  ZMaterialProperties materialProperties;
  materialProperties.albedo = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
  materialProperties.metallic = 0.f;
  materialProperties.roughness = 0.65f;
  materialProperties.ao = 0.2f;
  ZMaterial material(materialProperties);
  material.SetPBR();
  return material;
}
