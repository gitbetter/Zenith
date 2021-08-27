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
#include "ZFont.hpp"
#include "ZVertexBuffer.hpp"
#include "ZRenderTask.hpp"
#include "ZRenderPass.hpp"
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
    bufferData_ = ZVertexBuffer::Create(options);

    if (auto scene = Scene()) {
        options_.shader = ZServices::AssetStore()->TextShader();
        if (fontName_.empty()) fontName_ = "arial";
    }

    ZUIElement::Initialize();

    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.SetShader(options_.shader);
    writer.BindVertexBuffer(bufferData_);
    renderState_ = writer.End();
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

    Initialize();
}

std::shared_ptr<ZFont> ZUIText::Font()
{
    if (font_) return font_;

    auto scene = Scene();
    if (!scene) return nullptr;

    if (!ZServices::AssetStore()->HasFont(fontName_)) {
        LOG("The font " + fontName_ + " has not been loaded.", ZSeverity::Warning);
        return nullptr;
    }
    font_ = ZServices::AssetStore()->GetFont(fontName_);
    RecalculateBufferData();

    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.BindTexture(font_->Atlas().texture);
    renderState_ = writer.End();

    return font_;
}

void ZUIText::Prepare(double deltaTime, unsigned int zOrder)
{
    if (text_.empty() || options_.hidden) return;

    auto scene = Scene();
    if (!scene) return;

    auto font = Font();
    if (!font) return;

    ZPR_SESSION_COLLECT_VERTICES(textVertexData_.vertices.size());

    SetZOrder(zOrder);

    ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::Triangle);
    auto uiTask = ZRenderTask::Compile(drawCall,
        { renderState_ },
        ZRenderPass::UI()
    );
    uiTask->Submit({ ZRenderPass::UI() });
}

void ZUIText::OnRectChanged()
{
    ZUIElement::OnRectChanged();
    RecalculateBufferData();
}

void ZUIText::RecalculateBufferData()
{
    if (text_.empty() || !font_) return;

    auto pos = options_.calculatedRect.position;
    float x = pos.x, y = pos.y;

    float oneOverFontScale = 1.f / font_->Size();
    float fontSize = oneOverFontScale * fontScale_;
    float maxWrap = wrapToBounds_ ? MaxWrapBounds() : 0.f;
    float atlasH = font_->Atlas().height * fontSize;
    std::string text = text_;
    // If no wrapping is enabled we simulate banner scrolling by removing
    // characters from the beginning of the original string if the total width
    // of the text is greater than our container width.
    float width = 0.0f;
    auto it = text.rbegin();
    while (it != text.rend())
    {
        ZCharacter character = font_->Atlas().characterInfo[*it];
        width += character.advance.x * fontSize;
        if (width >= options_.calculatedRect.size.x)
            break;
        ++it;
    }
    if (it != text.rend() && !wrapToBounds_) {
        text = std::string(it.base(), text.end());
    }

    if (text.empty()) return;

    switch (hAlignment_) {
    case ZAlignment::Middle:
        x += glm::clamp(options_.calculatedRect.size.x - width, 0.f, options_.calculatedRect.size.x) * 0.5f;
        break;
    case ZAlignment::Right:
        x += glm::clamp(options_.calculatedRect.size.x - width, 0.f, options_.calculatedRect.size.x);
        break;
    default:
        break;
    }

    switch (vAlignment_) {
    case ZAlignment::Middle:
        y += glm::clamp(options_.calculatedRect.size.y - atlasH, 0.f, options_.calculatedRect.size.y) * 0.5f;
        break;
    case ZAlignment::Bottom:
        y += glm::clamp(options_.calculatedRect.size.y - atlasH, 0.f, options_.calculatedRect.size.y);
        break;
    default:
        break;
    }

    textVertexData_ = ZVertex2DDataOptions();
    textVertexData_.vertices.reserve(text.size() * 6);
    for (auto c = text.begin(); c != text.end(); c++)
    {
        ZCharacter character = font_->Atlas().characterInfo[*c];

        float w = character.bitmapSize.x * fontSize;
        float h = character.bitmapSize.y * fontSize;
        float xpos = x + character.bitmapPos.x * fontSize;
        float ypos = (y + atlasH) - character.bitmapPos.y * fontSize;

        x += character.advance.x * fontSize;
        y += character.advance.y * fontSize;

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
    if (text_ == text) return;
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

void ZUIText::SetHorizontalAlignment(ZAlignment alignment)
{
    hAlignment_ = alignment;
    RecalculateBufferData();
}

void ZUIText::SetVerticalAlignment(ZAlignment alignment)
{
    vAlignment_ = alignment;
    RecalculateBufferData();
}

DEFINE_UI_CREATORS(ZUIText)