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
#include "ZBuffer.hpp"
#include "ZDomain.hpp"

ZUIText::ZUIText(const std::string& text, const std::string& font, float fontSize, const glm::vec2& position, const glm::vec2& scale)
    : ZUIElement(position, scale), fontName_(font), text_(text), fontScale_(fontSize), lineSpacing_(0.f), wrapToBounds_(false)
{
    options_.enabled = false;
    type_ = ZUIElementType::Text;
}

ZUIText::ZUIText(const ZUIElementOptions& options, const std::string& text, const std::string& font, float fontSize)
    : ZUIElement(options), fontName_(font), text_(text), fontScale_(fontSize), lineSpacing_(0.f), wrapToBounds_(false)
{
    options_.enabled = false;
    type_ = ZUIElementType::Text;
}

void ZUIText::Initialize() {
    ZVertex2DDataOptions options;
    options.numVertices = 4;
    bufferData_ = ZBuffer::Create(options);

    if (auto scene = Scene()) {
        options_.shader = scene->AssetStore()->TextShader();
        if (fontName_.empty()) fontName_ = "arial";
    }

    ZUIElement::Initialize();
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
        fontName_ = fontProp->value;
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

std::shared_ptr<ZFont> ZUIText::Font()
{
    if (font_) return font_;

    auto scene = Scene();
    if (!scene) return nullptr;

    if (!scene->AssetStore()->HasFont(fontName_)) {
        LOG("The font " + fontName_ + " has not been loaded.", ZSeverity::Warning);
        return nullptr;
    }
    font_ = scene->AssetStore()->GetFont(fontName_);
    RecalculateBufferData();
    return font_;
}

void ZUIText::Draw()
{
    auto scene = Scene();
    if (!scene) return;

    auto font = Font();
    if (!font) return;

    if (text_.empty()) return;

    options_.shader->Activate();
    options_.shader->SetMat4("P", projectionMatrix_);
    options_.shader->SetVec4("color", options_.color);

    font->Atlas().texture->Bind(0);
    options_.shader->SetInt(font->Atlas().texture->type + "0", 0);

    ZServices::Graphics()->Draw(bufferData_, textVertexData_, ZMeshDrawStyle::Triangle);;
}

void ZUIText::OnRectChanged()
{
    RecalculateBufferData();
}

void ZUIText::RecalculateBufferData()
{
    if (text_.empty() || !font_) return;

    auto pos = options_.calculatedRect.position;
    float x = pos.x, y = pos.y;

    float maxWrap = wrapToBounds_ ? MaxWrapBounds() : 0.f;

    textVertexData_ = ZVertex2DDataOptions();
    textVertexData_.vertices.reserve(text_.size() * 6);
    for (auto c = text_.begin(); c != text_.end(); c++)
    {
        ZCharacter character = font_->Atlas().characterInfo[*c];

        float oneOverFontScale = 1.f / font_->Size();

        float atlasH = font_->Atlas().height * oneOverFontScale * fontScale_;
        float w = character.bitmapSize.x * oneOverFontScale * fontScale_;
        float h = character.bitmapSize.y * oneOverFontScale * fontScale_;
        float xpos = x + character.bitmapPos.x * oneOverFontScale * fontScale_;
        float ypos = (y + atlasH * 1.25f) - character.bitmapPos.y * oneOverFontScale * fontScale_;

        x += character.advance.x * oneOverFontScale * fontScale_;
        y += character.advance.y * oneOverFontScale * fontScale_;

        if (maxWrap > 0.f)
        {
            auto maxX = maxWrap - w * 2.f;
            if (x > maxX)
            {
                y += std::floor(x / maxX) * (atlasH + lineSpacing_);
                x = pos.x;
            }
        }

        if (w == 0 || h == 0) continue;

        textVertexData_.vertices.push_back(ZVertex2D(xpos, ypos, character.xOffset, 0));
        textVertexData_.vertices.push_back(ZVertex2D(xpos + w, ypos, character.xOffset + character.bitmapSize.x / font_->Atlas().width, 0));
        textVertexData_.vertices.push_back(ZVertex2D(xpos, ypos + h, character.xOffset, character.bitmapSize.y / font_->Atlas().height));
        textVertexData_.vertices.push_back(ZVertex2D(xpos + w, ypos, character.xOffset + character.bitmapSize.x / font_->Atlas().width, 0));
        textVertexData_.vertices.push_back(ZVertex2D(xpos, ypos + h, character.xOffset, character.bitmapSize.y / font_->Atlas().height));
        textVertexData_.vertices.push_back(ZVertex2D(xpos + w, ypos + h, character.xOffset + character.bitmapSize.x / font_->Atlas().width, character.bitmapSize.y / font_->Atlas().height));
    }

    bufferData_->Update(textVertexData_);
}

float ZUIText::MaxWrapBounds() const
{
    auto parent = Parent();
    return parent ? Position().x + parent->Size().x * 2.f : Position().x + Size().x;
}

void ZUIText::SetText(const std::string& text)
{
    text_ = text;
    RecalculateBufferData();
}

void ZUIText::SetFontScale(float scale)
{
    fontScale_ = scale;
    RecalculateBufferData();
}

void ZUIText::SetFont(const std::string& font)
{
    if (font.empty()) return;
    fontName_ = font;
    font_ = nullptr;
}

void ZUIText::SetWrap(bool wrap)
{
    wrapToBounds_ = wrap;
    RecalculateBufferData();
}

void ZUIText::SetLineSpacing(float spacing)
{
    lineSpacing_ = spacing;
    RecalculateBufferData();
}

DEFINE_UI_CREATORS(ZUIText)