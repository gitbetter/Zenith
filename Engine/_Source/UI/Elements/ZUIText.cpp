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
#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZAssetStore.hpp"
#include "ZShader.hpp"
#include "ZFont.hpp"

ZUIText::ZUIText(const std::string& text, const std::string& font, float fontSize, const glm::vec2& position, const glm::vec2& scale)
    : ZUIElement(position, scale), font_(font), text_(text), fontScale_(fontSize), wrapToBounds_(false)
{
    options_.enabled = false;
    type_ = ZUIElementType::Text;
}

ZUIText::ZUIText(const ZUIElementOptions& options, const std::string& text, const std::string& font, float fontSize)
    : ZUIElement(options), font_(font), text_(text), fontScale_(fontSize), wrapToBounds_(false)
{
    options_.enabled = false;
    type_ = ZUIElementType::Text;
}

void ZUIText::Initialize() {
    ZUIElement::Initialize();

    ZVertex2DDataOptions options;
    options.numVertices = 4;
    bufferData_ = ZBuffer::Create(options);

    if (auto scene = Scene()) {
        options_.shader = scene->AssetStore()->TextShader();
    }
}

void ZUIText::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    ZUIElement::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (node == nullptr)
    {
        LOG("Could not initalize ZUIElement", ZSeverity::Error);
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

    if (props.find("wrap") != props.end() && props["wrap"]->HasValues())
    {
        std::shared_ptr<ZOFString> textProp = props["wrap"]->Value<ZOFString>(0);
        wrapToBounds_ = textProp->value == "Yes";
    }

    if (props.find("lineSpacing") != props.end() && props["lineSpacing"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> lineSpacingProp = props["lineSpacing"]->Value<ZOFNumber>(0);
        lineSpacing_ = lineSpacingProp->value;
    }
}

void ZUIText::Draw()
{
    auto scene = Scene();
    if (!scene) return;

    options_.shader->Activate();
    options_.shader->SetMat4("M", projectionMatrix_);
    options_.shader->SetMat4("P", projectionMatrix_);
    options_.shader->SetVec4("color", options_.color);

    if (!scene->AssetStore()->HasFont(font_)) {
        LOG("The font " + font_ + " has not been loaded.", ZSeverity::Warning);
        return;
    }

    auto font = scene->AssetStore()->GetFont(font_);
    font->Atlas().texture->Bind(0);
    options_.shader->SetInt(font->Atlas().texture->type + "0", 0);

    ZServices::Graphics()->DrawText(bufferData_, options_.calculatedRect.position, text_, font,
        fontScale_, lineSpacing_, wrapToBounds_ ? MaxWrapBounds() : 0.f);
}

float ZUIText::MaxWrapBounds() const
{
    auto parent = Parent();
    return parent ? Position().x + parent->Size().x * 2.f : Position().x + Size().x;
}

DEFINE_UI_CREATORS(ZUIText)