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
#include "ZShaderReadyEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZEventAgent.hpp"

ZGraphicsFactory::ZGraphicsFactory()
{
    modelCreators_["Cube"] = &ZModel::NewCubePrimitive;
    modelCreators_["Plane"] = &ZModel::NewPlanePrimitive;
    modelCreators_["Sphere"] = &ZModel::NewSpherePrimitive;
    modelCreators_["Cylinder"] = &ZModel::NewCylinderPrimitive;
    modelCreators_["Cone"] = &ZModel::NewConePrimitive;
}

void ZGraphicsFactory::CreateShadersAsync(std::shared_ptr<ZOFTree> data, std::map<std::shared_ptr<ZShader>, std::string>& outPendingShaders)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZSH") == 0)
        {
            std::string vertexPath = "", pixelPath = "", geometryPath = "";

            std::shared_ptr<ZOFObjectNode> shaderNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++)
            {
                if (!it->second->HasValues()) continue;

                std::shared_ptr<ZOFString> str = it->second->Value<ZOFString>(0);
                if (it->second->id == "vertex") vertexPath = str->value;
                else if (it->second->id == "pixel") pixelPath = str->value;
                else if (it->second->id == "geometry") geometryPath = str->value;
            }

            std::shared_ptr<ZShader> shader(new ZShader(vertexPath, pixelPath, geometryPath));
            outPendingShaders[shader] = it->first;
        }
    }

    for (auto it = outPendingShaders.begin(); it != outPendingShaders.end(); it++)
    {
        it->first->InitializeAsync();
    }
}

void ZGraphicsFactory::CreateShaders(std::shared_ptr<ZOFTree> data, ZShaderMap& outShaderMap)
{
    ZShaderMap shaders;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZSH") == 0)
        {
            std::string vertexPath = "", pixelPath = "", geometryPath = "";

            std::shared_ptr<ZOFObjectNode> shaderNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++)
            {
                if (!it->second->HasValues()) continue;

                std::shared_ptr<ZOFString> str = it->second->Value<ZOFString>(0);
                if (it->second->id == "vertex") vertexPath = str->value;
                else if (it->second->id == "pixel") pixelPath = str->value;
                else if (it->second->id == "geometry") geometryPath = str->value;
            }

            std::shared_ptr<ZShader> shader(new ZShader(vertexPath, pixelPath, geometryPath));
            shader->Initialize();
            shaders[it->first] = shader;
        }
    }
    outShaderMap = shaders;
}

void ZGraphicsFactory::CreateTexturesAsync(std::shared_ptr<ZOFTree> data, std::map<std::string, std::string>& outPendingTextures)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0)
        {
            std::string path;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (!path.empty())
            {
                outPendingTextures[path] = it->first;
            }
        }
    }

    for (auto it = outPendingTextures.begin(); it != outPendingTextures.end(); it++)
    {
        zenith::Graphics()->Strategy()->LoadTextureAsync(it->first, "");
    }
}

void ZGraphicsFactory::CreateTextures(std::shared_ptr<ZOFTree> data, ZTextureMap& outTextureMap)
{
    ZTextureMap textures;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0)
        {
            std::string path;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (!path.empty())
            {
                ZTexture texture = zenith::Graphics()->Strategy()->LoadTexture(path, "");
                textures[it->first] = texture;
            }
        }
    }
    outTextureMap = textures;
}

void ZGraphicsFactory::CreateModelsAsync(std::shared_ptr<ZOFTree> data, ZModelIDMap& outPendingModels)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZMOD") == 0)
        {
            std::string path;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (!path.empty())
            {
                std::shared_ptr<ZModel> model(new ZModel(path));
                outPendingModels[model] = it->first;
            }
        }
    }

    for (auto it = outPendingModels.begin(); it != outPendingModels.end(); it++)
    {
        it->first->InitializeAsync();
    }
}

void ZGraphicsFactory::CreateModels(std::shared_ptr<ZOFTree> data, ZModelMap& outModelMap)
{
    ZModelMap models;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZMOD") == 0)
        {
            std::string path;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (!path.empty())
            {
                std::shared_ptr<ZModel> model(new ZModel(path));
                model->Initialize();
                models[it->first] = model;
            }
        }
    }
    outModelMap = models;
}

// TODO: Some repeated code across these functions. Refactor it.
void ZGraphicsFactory::CreateAssetsAsync(std::shared_ptr<ZOFTree> data, ZTextureTypeMap& outPendingTextures, ZShaderIDMap& outPendingShaders, ZModelIDMap& outPendingModels)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0 || it->first.find("ZMOD") == 0)
        {
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            std::string path;
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (!path.empty())
            {
                if (it->first.find("ZTEX") == 0)
                {
                    outPendingTextures[path] = it->first;
                }
                else if (it->first.find("ZMOD") == 0)
                {
                    std::shared_ptr<ZModel> model(new ZModel(path));
                    outPendingModels[model] = it->first;
                }
            }
        }
        else if (it->first.find("ZSH") == 0)
        {
            std::shared_ptr<ZOFObjectNode> shaderNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            std::string vertexPath = "", pixelPath = "", geometryPath = "";
            for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++)
            {
                if (!it->second->HasValues()) continue;

                std::shared_ptr<ZOFString> str = it->second->Value<ZOFString>(0);
                if (it->second->id == "vertex") vertexPath = str->value;
                else if (it->second->id == "pixel") pixelPath = str->value;
                else if (it->second->id == "geometry") geometryPath = str->value;
            }

            std::shared_ptr<ZShader> shader(new ZShader(vertexPath, pixelPath, geometryPath));
            outPendingShaders[shader] = it->first;
        }
    }

    for (auto it = outPendingTextures.begin(); it != outPendingTextures.end(); it++)
    {
        zenith::Graphics()->Strategy()->LoadTextureAsync(it->first, "");
    }

    for (auto it = outPendingShaders.begin(); it != outPendingShaders.end(); it++)
    {
        it->first->InitializeAsync();
    }

    for (auto it = outPendingModels.begin(); it != outPendingModels.end(); it++)
    {
        it->first->InitializeAsync();
    }
}

void ZGraphicsFactory::CreateAssets(std::shared_ptr<ZOFTree> data, ZTextureMap& outTextureMap, ZShaderMap& outShaderMap, ZModelMap& outModelMap)
{
    ZTextureMap textures; ZShaderMap shaders; ZModelMap models;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0 || it->first.find("ZMOD") == 0)
        {
            std::string path;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (!path.empty())
            {
                if (it->first.find("ZTEX") == 0)
                {
                    ZTexture texture = zenith::Graphics()->Strategy()->LoadTexture(path, "");
                    textures[it->first] = texture;
                }
                else if (it->first.find("ZMOD") == 0)
                {
                    std::shared_ptr<ZModel> model(new ZModel(path));
                    model->Initialize();
                    outModelMap[it->first] = model;
                }
            }
        }
        else if (it->first.find("ZSH") == 0)
        {
            std::string vertexPath = "", pixelPath = "", geometryPath = "";

            std::shared_ptr<ZOFObjectNode> shaderNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            for (ZOFPropertyMap::iterator it = shaderNode->properties.begin(); it != shaderNode->properties.end(); it++)
            {
                if (!it->second->HasValues()) continue;

                std::shared_ptr<ZOFString> str = it->second->Value<ZOFString>(0);
                if (it->second->id == "vertex") vertexPath = str->value;
                else if (it->second->id == "pixel") pixelPath = str->value;
                else if (it->second->id == "geometry") geometryPath = str->value;
            }

            std::shared_ptr<ZShader> shader(new ZShader(vertexPath, pixelPath, geometryPath));
            shader->Initialize();
            shaders[it->first] = shader;
        }
    }
    outTextureMap = textures; outShaderMap = shaders; outModelMap = models;
}

std::unique_ptr<ZModel> ZGraphicsFactory::CreateModel(std::string type, glm::vec3 scale)
{
    return modelCreators_[type](scale);
}
