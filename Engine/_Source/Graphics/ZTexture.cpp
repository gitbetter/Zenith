/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZTexture.cpp

    Created by Adrian Sanchez on 24/01/2021.
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

#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZTexture.hpp"
#include "ZOFTree.hpp"

void ZTexture::CreateAsync(std::shared_ptr<ZOFTree> data, ZTextureMap& outPendingTextures)
{
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0)
        {
            std::string path;
            ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (textureNode->properties.find("wrapping") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> wrappingStr = textureNode->properties["wrapping"]->Value<ZOFString>(0);
                wrapping = wrappingStr->value == "Repeat" ? ZTextureWrapping::Repeat : ZTextureWrapping::EdgeClamp;
            }

            if (!path.empty())
            {
                ZTexture texture;
                texture.path = path;
                texture.name = it->first;
                texture.wrapping = wrapping;
                outPendingTextures[path] = texture;
            }
        }
    }

    for (auto it = outPendingTextures.begin(); it != outPendingTextures.end(); it++)
    {
        zenith::Graphics()->LoadTextureAsync(it->first, "", it->second.wrapping);
    }
}

void ZTexture::Create(std::shared_ptr<ZOFTree> data, ZTextureMap& outTextureMap)
{
    ZTextureMap textures;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        if (it->first.find("ZTEX") == 0)
        {
            std::string path;
            ZTextureWrapping wrapping = ZTextureWrapping::EdgeClamp;
            std::shared_ptr<ZOFObjectNode> textureNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
            if (textureNode->properties.find("path") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> pathStr = textureNode->properties["path"]->Value<ZOFString>(0);
                path = pathStr->value;
            }

            if (textureNode->properties.find("wrapping") != textureNode->properties.end())
            {
                std::shared_ptr<ZOFString> wrappingStr = textureNode->properties["wrapping"]->Value<ZOFString>(0);
                wrapping = wrappingStr->value == "Repeat" ? ZTextureWrapping::Repeat : ZTextureWrapping::EdgeClamp;
            }

            if (!path.empty())
            {
                ZTexture texture = zenith::Graphics()->LoadTexture(path, "", wrapping);
                textures[it->first] = texture;
            }
        }
    }
    outTextureMap = textures;
}