//
//  ZGraphicsFactory.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 19/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
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
      for (ZOFPropertyNode* prop : shaderNode->properties) {
        if (prop->values.size() == 0) continue;

        ZOFString* terminal = dynamic_cast<ZOFString*>(prop->values[0]);
        std::string path = terminal->value.find("\"") == 0 ? terminal->value.substr(1, terminal->value.size() - 2) : terminal->value;
        if (prop->id == "vertex") vertexPath = path;
        else if (prop->id == "pixel") pixelPath = path;
        else if (prop->id == "geometry") geometryPath = path;
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
      //ZOFNode* textureNode = it->second;
      // TODO:
    }
  }
  return textures;
}

std::unique_ptr<ZModel> ZGraphicsFactory::CreateModel(std::string type, glm::vec3 scale, std::vector<ZTexture> textures) {
  return modelCreators_[type](scale, textures);
}
