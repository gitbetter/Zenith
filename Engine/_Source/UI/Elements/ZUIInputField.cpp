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

ZUIInputField::ZUIInputField() : ZUIElement()
{
    type = ZUIElementType::InputField;
}

void ZUIInputField::OnInitialize() {
    CreateTextArea();

    ZServices::EventAgent()->Subscribe(this, &ZUIInputField::HandleKeyPressed);
    ZServices::EventAgent()->Subscribe(this, &ZUIInputField::HandleButtonPressed);
}

void ZUIInputField::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    ZOFPropertyMap& props = dataNode->properties;

    if (props.find("placeholder") != props.end() && props["placeholder"]->HasValues())
    {
        std::shared_ptr<ZOFString> placeholderProp = props["placeholder"]->Value<ZOFString>(0);
        placeholder_ = placeholderProp->value;
    }
}

void ZUIInputField::SetText(const std::string& text)
{
    text_ = text;
    if (!fieldText_.IsNull()) {
        ZUIText* textElement = ZServices::UIElementManager()->Dereference<ZUIText>(fieldText_);
        textElement->SetText(text_);
    }
    if (onInputChangedCallback_) {
        onInputChangedCallback_(text_);
    }
}

void ZUIInputField::SetTextColor(const glm::vec4& color)
{
    textColor_ = color;
    if (!fieldText_.IsNull()) {
        ZServices::UIElementManager()->SetColor(fieldText_, textColor_);
    }
}

void ZUIInputField::SetHighlightBorder(const ZUIBorder& border)
{
    highlightBorder_ = border;
    if (focused_) {
        ZServices::UIElementManager()->SetBorder(handle, highlightBorder_);
    }
}

void ZUIInputField::SetFocused(bool focused)
{
    focused_ = focused;
    if (focused_) {
        ZServices::UIElementManager()->SetBorder(handle, highlightBorder_);
    }
    else {
        ZUIBorder border = highlightBorder_;
        border.width = 0.f;
        ZServices::UIElementManager()->SetBorder(handle, border);
    }
}

void ZUIInputField::SetBackground(const ZHTexture& texture)
{
    ZServices::UIElementManager()->SetTexture(handle, texture);
}

void ZUIInputField::SetBackground(const glm::vec4& color)
{
    ZServices::UIElementManager()->SetColor(handle, color);
}

void ZUIInputField::SetFontSize(float size)
{
    fontSize_ = size;
    if (!fieldText_.IsNull()) {
        ZUIText* textElement = ZServices::UIElementManager()->Dereference<ZUIText>(fieldText_);
        textElement->SetFontScale(fontSize_);
    }
}

void ZUIInputField::SetFieldPadding(const glm::vec2& padding)
{
    fieldPadding_ = padding;
    if (!fieldText_.IsNull()) {
        ZServices::UIElementManager()->SetRect(fieldText_, ZRect(fieldPadding_.x * 2.f, fieldPadding_.y * 2.f, 1.f - fieldPadding_.x, 1.f - fieldPadding_.y));
    }
}

void ZUIInputField::CreateTextArea()
{
    auto scene = ZServices::UIElementManager()->Scene(handle);
    if (!scene) return;

    ZUIElementOptions fieldTextOptions;
    fieldTextOptions.positioning = ZPositioning::Relative;
    fieldTextOptions.scaling = ZPositioning::Relative;
    fieldTextOptions.rect = ZRect(fieldPadding_.x * 2.f, fieldPadding_.y * 2.f, 1.f - fieldPadding_.x, 1.f - fieldPadding_.y);
    fieldTextOptions.color = textColor_;
    fieldText_ = ZUIText::Create(fieldTextOptions, scene);
    ZUIText* textElement = ZServices::UIElementManager()->Dereference<ZUIText>(fieldText_);
    textElement->SetFontScale(fontSize_);

    ZServices::UIElementManager()->AddChild(handle, fieldText_);
}

void ZUIInputField::ProcessKey(const ZKey& key)
{
    std::string newText = text_;
    if (key == ZKey::BACKSPACE) {
        if (!newText.empty()) newText.pop_back();
    }
    else {
        if (PRINTABLE_KEY_CODE.find(key) != PRINTABLE_KEY_CODE.end()) {
            char c = PRINTABLE_KEY_CODE.at(key);
            if (filter_ && filter_(c)) {
                newText += PRINTABLE_KEY_CODE.at(key);
            }
        }
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
        SetFocused(ZServices::UIElementManager()->Contains(handle, ZServices::Input()->GetCursorPosition()));
    }
}

DEFINE_UI_CREATORS(ZUIInputField)