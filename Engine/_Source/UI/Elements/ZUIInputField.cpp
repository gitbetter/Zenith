/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIInputField.hpp

    Created by Adrian Sanchez on 02/07/2021.
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

#include "ZUIInputField.hpp"
#include "ZInputKeyEvent.hpp"
#include "ZInputButtonEvent.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZUIText.hpp"
#include "ZUIPanel.hpp"
#include "ZServices.hpp"

ZUIInputField::ZUIInputField(const std::string& label, const std::string& placeholder, const glm::vec2& position, const glm::vec2& scale)
    : ZUIElement(position, scale), label_(label), placeholder_(placeholder), textColor_(1.f), labelTextColor_(1.f), fieldPadding_(0.02f, 0.f)
{
}

ZUIInputField::ZUIInputField(const ZUIElementOptions& options, const std::string& label, const std::string& placeholder)
    : ZUIElement(options), label_(label), placeholder_(placeholder), textColor_(1.f), labelTextColor_(1.f), fieldPadding_(0.02f, 0.f)
{
}

void ZUIInputField::Initialize() {
    ZUIElement::Initialize();

    auto scene = Scene();
    if (!scene)
    {
        LOG("ZUIInputField was not attached to scene during initialization", ZSeverity::Error);
        return;
    }

    ZUILayoutOptions layoutOptions;
    layoutOptions.verticalAlign = ZAlignment::Middle;
    layoutOptions.dimensions = options_.calculatedRect;
    options_.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);

    CreateLabelField(label_);
    CreateField();

    ZServices::EventAgent()->Subscribe(this, &ZUIInputField::HandleKeyPressed);
    ZServices::EventAgent()->Subscribe(this, &ZUIInputField::HandleButtonPressed);
}

void ZUIInputField::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    ZUIElement::Initialize(root);

    auto node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZUIInputField", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("placeholder") != props.end() && props["placeholder"]->HasValues())
    {
        std::shared_ptr<ZOFString> placeholderProp = props["placeholder"]->Value<ZOFString>(0);
        placeholder_ = placeholderProp->value;
    }

    Initialize();
}

void ZUIInputField::SetLabel(const std::string& label)
{
    labelText_->SetText(label);
    float labelWidth = label.empty() ? 0.f : labelWidth_;
    labelText_->SetRect(ZRect(0.f, 0.f, labelWidth, 1.f), options_.calculatedRect);
    field_->SetRect(ZRect(0.f, 0.f, 1.f - labelWidth, 1.f), options_.calculatedRect);
}

void ZUIInputField::SetLabelWidth(float width)
{
    labelWidth_ = width;
    if (labelText_) {
        labelText_->SetRect(ZRect(0.f, 0.f, labelWidth_, 1.f), options_.calculatedRect);
        field_->SetRect(ZRect(0.f, 0.f, 1.f - labelWidth_, 1.f), options_.calculatedRect);
    }
}

void ZUIInputField::SetText(const std::string& text)
{
    text_ = text;
    if (fieldText_) {
        fieldText_->SetText(text_);
    }
}

void ZUIInputField::SetTextColor(const glm::vec4& color)
{
    textColor_ = color;
    if (fieldText_) {
        fieldText_->SetColor(textColor_);
    }
}

void ZUIInputField::SetLabelTextColor(const glm::vec4& color)
{
    labelTextColor_ = color;
    if (labelText_) {
        labelText_->SetColor(labelTextColor_);
    }
}

void ZUIInputField::SetHighlightBorder(const ZUIBorder& border)
{
    highlightBorder_ = border;
    if (focused_) {
        field_->SetBorder(highlightBorder_);
    }
}

void ZUIInputField::SetFocused(bool focused)
{
    focused_ = focused;
    if (focused_) {
        field_->SetBorder(highlightBorder_);
    }
    else {
        ZUIBorder border = highlightBorder_;
        border.width = 0.f;
        field_->SetBorder(border);
    }
}

void ZUIInputField::SetBackground(const ZTexture::ptr& texture)
{
    if (field_) {
        field_->SetTexture(texture);
    }
}

void ZUIInputField::SetBackground(const glm::vec4& color)
{
    if (field_) {
        field_->SetColor(color);
    }
}

void ZUIInputField::SetFontSize(float size)
{
    fontSize_ = size;
    if (labelText_) {
        labelText_->SetFontScale(fontSize_);
    }
    if (fieldText_) {
        fieldText_->SetFontScale(fontSize_);
    }
}

void ZUIInputField::SetFieldPadding(const glm::vec2& padding)
{
    fieldPadding_ = padding;
    if (fieldText_) {
        fieldText_->SetRect(ZRect(fieldPadding_.x * 2.f, fieldPadding_.y * 2.f, 1.f - fieldPadding_.x, 1.f - fieldPadding_.y));
    }
}

void ZUIInputField::CreateLabelField(const std::string& text)
{
    auto scene = Scene();
    if (!scene) return;

    ZUIElementOptions textOptions;
    textOptions.positioning = ZPositioning::Relative;
    textOptions.scaling = ZPositioning::Relative;
    textOptions.rect = ZRect(0.f, 0.f, text.empty() ? 0.f : labelWidth_, 1.f);
    textOptions.color = labelTextColor_;
    labelText_ = ZUIText::Create(textOptions, scene);
    if (!text.empty())
        labelText_->SetText(text);
    labelText_->SetFontScale(fontSize_);

    AddChild(labelText_);
}

void ZUIInputField::CreateField()
{
    auto scene = Scene();
    if (!scene) return;

    ZUIElementOptions fieldOptions;
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.f, 0.f, 1.f - labelWidth_, 1.f);
    fieldOptions.color = glm::vec4(options_.color + 0.3f);
    fieldOptions.color.a = 1.f;
    field_ = ZUIPanel::Create(fieldOptions, scene);

    AddChild(field_);

    ZUIElementOptions fieldTextOptions;
    fieldTextOptions.positioning = ZPositioning::Relative;
    fieldTextOptions.scaling = ZPositioning::Relative;
    fieldTextOptions.rect = ZRect(fieldPadding_.x * 2.f, fieldPadding_.y * 2.f, 1.f - fieldPadding_.x, 1.f - fieldPadding_.y);
    fieldTextOptions.color = textColor_;
    fieldText_ = ZUIText::Create(fieldTextOptions, scene);
    fieldText_->SetFontScale(fontSize_);

    field_->AddChild(fieldText_);
}

void ZUIInputField::DestroyLabelField()
{
    if (labelText_) {
        RemoveChild(labelText_);
        labelText_->CleanUp();
        labelText_ = nullptr;
    }
}

void ZUIInputField::ProcessKey(const ZKey& key)
{
    std::string newText = text_;
    if (key == ZKey::BACKSPACE) {
        if (!newText.empty()) newText.pop_back();
    }
    else {
        if (PRINTABLE_KEY_CODE.find(key) != PRINTABLE_KEY_CODE.end())
            newText += PRINTABLE_KEY_CODE.at(key);
    }
    SetText(newText);
}

void ZUIInputField::HandleKeyPressed(const std::shared_ptr<ZInputKeyEvent>& event)
{
    if (event->Pressed() && focused_) {
        ProcessKey(event->Key());
    }
}

void ZUIInputField::HandleButtonPressed(const std::shared_ptr<ZInputButtonEvent>& event)
{
    if (event->Pressed() && event->Button() == ZMouse::LEFT_MB) {
        SetFocused(Contains(ZServices::Input()->GetCursorPosition()));
    }
}

DEFINE_UI_CREATORS(ZUIInputField)