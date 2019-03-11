//
//  ZGraphicsFactory.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 19/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZGraphicsFactory.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"

ZGraphicsFactory::ZGraphicsFactory() {
  modelCreators_["Cube"] = &ZModel::NewCubePrimitive;
  modelCreators_["Plane"] = &ZModel::NewPlanePrimitive;
  modelCreators_["Sphere"] = &ZModel::NewSpherePrimitive;
  modelCreators_["Cylinder"] = &ZModel::NewCylinderPrimitive;
  modelCreators_["Cone"] = &ZModel::NewConePrimitive;
}

ZShaderMap ZGraphicsFactory::CreateShaders(ZOFTree* data) {
  ZShaderMap shaders;
  for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
    if (it->first.find("ZSH") == 0) {
      std::string vertexPath = "", pixelPath = "", geometryPath = "";
      std::shared_ptr<ZShader> shader(new ZShader);

      ZOFObjectNode* shaderNode = dynamic_cast<ZOFObjectNode*>(it->second);

      for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++) {
        if (!it->second->HasValues()) continue;

        ZOFString* str = it->second->Value<ZOFString>(0);
        if (it->second->id == "vertex") vertexPath = str->value;
        else if (it->second->id == "pixel") pixelPath = str->value;
        else if (it->second->id == "geometry") geometryPath = str->value;
      }

      shader->Initialize(vertexPath, pixelPath, geometryPath);
      shaders[it->first] = shader;
    }
  }
  return shaders;
}

ZTextureMap ZGraphicsFactory::CreateTextures(ZOFTree* data) {
  ZTextureMap textures;
  for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
    if (it->first.find("ZTEX") == 0) {
      std::string path;
      ZOFObjectNode* textureNode = dynamic_cast<ZOFObjectNode*>(it->second);
      if (textureNode->properties.find("path") != textureNode->properties.end()) {
        ZOFString* pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
        path = pathStr->value;
      }

      if (!path.empty()) {
        ZTexture texture = ZEngine::Graphics()->Strategy()->LoadTexture(path, "");
        textures[it->first] = texture;
      }
    }
  }
  return textures;
}

std::unique_ptr<ZModel> ZGraphicsFactory::CreateModel(std::string type, glm::vec3 scale) {
  return modelCreators_[type](scale);
}
