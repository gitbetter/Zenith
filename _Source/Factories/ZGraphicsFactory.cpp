/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGraphicsFactory.cpp

    Created by Adrian Sanchez on 19/02/2019.
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

ZShaderMap ZGraphicsFactory::CreateShaders(std::shared_ptr<ZOFTree> data) {
  ZShaderMap shaders;
  for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
    if (it->first.find("ZSH") == 0) {
      std::string vertexPath = "", pixelPath = "", geometryPath = "";
      std::shared_ptr<ZShader> shader(new ZShader);

      std::shared_ptr<ZOFObjectNode> shaderNode = std::dynamic_pointer_cast<ZOFObjectNode>(it->second);

      for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++) {
        if (!it->second->HasValues()) continue;

        std::shared_ptr<ZOFString> str = it->second->Value<ZOFString>(0);
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

ZTextureMap ZGraphicsFactory::CreateTextures(std::shared_ptr<ZOFTree> data) {
  ZTextureMap textures;
  for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
    if (it->first.find("ZTEX") == 0) {
      std::string path;
      std::shared_ptr<ZOFObjectNode> textureNode = std::dynamic_pointer_cast<ZOFObjectNode>(it->second);
      if (textureNode->properties.find("path") != textureNode->properties.end()) {
        std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
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