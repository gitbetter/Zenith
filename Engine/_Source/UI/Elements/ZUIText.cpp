/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIText.cpp

    Created by Adrian Sanchez on 11/02/2019.
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

#include "ZUIText.hpp"
#include "ZUI.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZTextStrategy.hpp"
#include "ZShader.hpp"
#include "ZDomain.hpp"

ZUIText::ZUIText(const std::string& text, const std::string& font, float fontSize, const glm::vec2& position, const glm::vec2& scale)
    : ZUIElement(position, scale)
{
    font_ = font;
    text_ = text;
    fontScale_ = fontSize;
    enabled_ = false;
    type_ = ZUIElementType::Text;
}

void ZUIText::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    ZVertex2DDataOptions options;
    options.numVertices = 4;
    bufferData_ = zenith::Graphics()->Strategy()->LoadVertexData(options);

    ZUIElement::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (node == nullptr)
    {
        zenith::Log("Could not initalize ZUIElement", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("font") != props.end() && props["font"]->HasValues())
    {
        std::shared_ptr<ZOFString> fontProp = props["font"]->Value<ZOFString>(0);
        font_ = fontProp->value;
        if (props["font"]->ValueCount() > 1)
        {
            std::shared_ptr<ZOFNumber> fontSizeProp = props["font"]->Value<ZOFNumber>(1);
            fontScale_ = fontSizeProp->value;
        }
    }

    if (props.find("text") != props.end() && props["text"]->HasValues())
    {
        std::shared_ptr<ZOFString> textProp = props["text"]->Value<ZOFString>(0);
        text_ = textProp->value;
    }
}
