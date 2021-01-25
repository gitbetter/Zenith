/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIImage.cpp

    Created by Adrian Sanchez on 06/02/2019.
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

#include "ZUI.hpp"
#include "ZUIImage.hpp"
#include "ZShader.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZEventAgent.hpp"

ZUIImage::ZUIImage(const std::string& path, const glm::vec2& position, const glm::vec2& scale) : ZUIElement(position, scale)
{
    options_.enabled = false;
    path_ = path;
    type_ = ZUIElementType::Image;
    SetImage(path_);
}

ZUIImage::ZUIImage(const ZUIElementOptions& options, const std::string& path) : ZUIElement(options)
{
    options_.enabled = false;
    path_ = path;
    type_ = ZUIElementType::Image;
    SetImage(path_);
}

void ZUIImage::Initialize()
{
    ZUIElement::Initialize();
}

void ZUIImage::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    ZUIElement::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (node == nullptr)
    {
        zenith::Log("Could not initalize ZUIImage", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("path") != props.end() && props["path"]->HasValues())
    {
        std::shared_ptr<ZOFString> pathProp = props["path"]->Value<ZOFString>(0);
        path_ = pathProp->value;
        SetImage(path_);
    }
}

void ZUIImage::SetImage(const std::string& path)
{
    if (!path.empty())
    {
        ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZUIImage::HandleTextureReady);
        zenith::EventAgent()->AddListener(textureReadyDelegate, ZTextureReadyEvent::Type);
        zenith::Graphics()->LoadTextureAsync(path, "");
    }
}

void ZUIImage::HandleTextureReady(const std::shared_ptr<ZEvent>& event)
{
    std::shared_ptr<ZTextureReadyEvent> textureReadyEvent = std::static_pointer_cast<ZTextureReadyEvent>(event);
    if (textureReadyEvent->Texture().path == path_)
    {
        options_.texture = textureReadyEvent->Texture();
        options_.texture.type = "image";

        ZEventDelegate textureReadyDelegate = fastdelegate::MakeDelegate(this, &ZUIImage::HandleTextureReady);
        zenith::EventAgent()->RemoveListener(textureReadyDelegate, ZTextureReadyEvent::Type);
    }
}
