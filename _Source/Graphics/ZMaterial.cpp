//
//  ZMaterial.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 01/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZMaterial.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"

void ZMaterial::Initialize(ZOFTree* root) {
  std::vector<ZTexture> textures;
  ZMaterialProperties materialProperties;
  bool isPBR = false, hasDisplacement = false;

  ZOFObjectNode* materialNode = dynamic_cast<ZOFObjectNode*>(root);

  for (ZOFPropertyMap::iterator it = materialNode->properties.begin(); it != materialNode->properties.end(); it++) {
    if (!it->second->HasValues()) continue;

    if (!isPBR)
      isPBR = it->second->id == "metallic" || it->second->id == "roughness" || it->second->id == "ao";

    if (!hasDisplacement)
      hasDisplacement = it->second->id == "height";

    if (ZOFString* strProp = it->second->Value<ZOFString>(0)) {
      if (ZEngine::Graphics()->Textures().find(strProp->value) != ZEngine::Graphics()->Textures().end()) {
        ZTexture texture = ZEngine::Graphics()->Textures()[strProp->value];
        texture.type = it->second->id;
        textures.push_back(texture);
      }
    } else if (ZOFNumber* numProp = it->second->Value<ZOFNumber>(0)) {
      SetMaterialProperty(it->second->id, numProp->value, materialProperties);
    } else if (ZOFNumberList* numListProp = it->second->Value<ZOFNumberList>(0)) {
      SetMaterialProperty(it->second->id, glm::vec4(numListProp->value[0], numListProp->value[1], numListProp->value[2], 1.0), materialProperties);
    }
  }

  isPBR_ = isPBR;
  hasDisplacement_ = hasDisplacement;

  if (!textures.empty()) textures_ = textures;
  else properties_ = materialProperties;
}

std::unique_ptr<ZMaterial> ZMaterial::DefaultMaterialSimple() {
  ZMaterialProperties materialProperties;
  materialProperties.albedo = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
  materialProperties.emission = 0.f;
  materialProperties.diffuse = 0.5f;
  materialProperties.ambient = 0.3f;
  materialProperties.specular = 0.2f;
  materialProperties.shininess = 0.2f;
  std::unique_ptr<ZMaterial> material(new ZMaterial(materialProperties));
  return material;
}

std::unique_ptr<ZMaterial> ZMaterial::DefaultMaterialPBR() {
  ZMaterialProperties materialProperties;
  materialProperties.albedo = glm::vec4(0.9f, 0.9f, 0.9f, 1.f);
  materialProperties.metallic = 0.1f;
  materialProperties.roughness = 0.75f;
  materialProperties.ao = 0.3f;
  std::unique_ptr<ZMaterial> material(new ZMaterial(materialProperties));
  material->SetPBR();
  return material;
}

void ZMaterial::SetMaterialProperty(std::string property, float value, ZMaterialProperties& materialProperties) {
  if (property == "emission") {
    materialProperties.emission = value;
  } else if (property == "diffuse") {
    materialProperties.diffuse = value;
  } else if (property == "ambient") {
    materialProperties.ambient = value;
  } else if (property == "specular") {
    materialProperties.specular = value;
  } else if (property == "shininess") {
    materialProperties.shininess = value;
  } else if (property == "metallic") {
    materialProperties.metallic = value;
  } else if (property == "roughness") {
    materialProperties.roughness = value;
  } else if (property == "ao") {
    materialProperties.ao = value;
  }
}

void ZMaterial::SetMaterialProperty(std::string property, glm::vec4 value, ZMaterialProperties& materialProperties) {
  if (property == "albedo") {
    materialProperties.albedo = value;
  }
}
