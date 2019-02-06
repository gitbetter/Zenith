//
//  ZMaterial.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 01/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZMaterial.hpp"

ZMaterial ZMaterial::DefaultMaterial() {
  ZMaterialProperties materialProperties;
  materialProperties.albedo = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);
  materialProperties.emission = glm::vec3(0.f);
  materialProperties.diffuse = glm::vec3(0.4f);
  materialProperties.ambient = glm::vec3(0.2f);
  materialProperties.specular = glm::vec3(0.2f);
  materialProperties.shininess = 0.5f;
  return ZMaterial(materialProperties);
}
