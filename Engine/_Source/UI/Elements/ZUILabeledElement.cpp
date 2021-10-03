/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUILabeledElement.hpp

    Created by Adrian Sanchez on 02/16/2021.
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

#include "ZUILabeledElement.hpp"
#include "ZServices.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZUIText.hpp"
#include "ZUIPanel.hpp"

ZUILabeledElement::ZUILabeledElement() : ZUIElement()
{
    // A labeled element is just a wrapper/decorator object, so it should inherit most of the
    // properties of the wrapped element
    options.color = glm::vec4(0.f);
}

void ZUILabeledElement::OnInitialize()
{
    ZUILayoutOptions layoutOptions;
    layoutOptions.verticalAlign = ZAlignment::Middle;
    layoutOptions.dimensions = options.calculatedRect;
    options.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);

    // A horizontal layout will layout elements based on sequential calls to
    // AddChild on the layout container. If we want to change the label position
    // we just change the order of the AddChild calls.
    if (labelPosition_ == Position::Left) {
        CreateLabelField();
        SetupElement();
    }
    else {
        SetupElement();
        CreateLabelField();
    }
}

void ZUILabeledElement::SetLabel(const std::string& label)
{
    ZUIText* textLabel = ZServices::UIElementManager()->Dereference<ZUIText>(labelText_);
    textLabel->SetText(label);
    float labelWidth = label.empty() ? 0.f : labelWidth_;
    ZServices::UIElementManager()->SetRect(labelText_, ZRect(0.f, 0.f, labelWidth, 1.f), options.calculatedRect);
    ZServices::UIElementManager()->SetRect(element_, ZRect(0.f, 0.f, 1.f - labelWidth, 1.f), options.calculatedRect);
}

void ZUILabeledElement::SetLabelWidth(float width)
{
    labelWidth_ = width;
    if (!labelText_.IsNull()) {
        ZServices::UIElementManager()->SetRect(labelText_, ZRect(0.f, 0.f, labelWidth_, 1.f), options.calculatedRect);
        ZServices::UIElementManager()->SetRect(element_, ZRect(0.f, 0.f, 1.f - labelWidth_, 1.f), options.calculatedRect);
    }
}

void ZUILabeledElement::SetLabelFontSize(float size)
{
    labelFontSize_ = size;
    if (!labelText_.IsNull()) {
        ZUIText* textLabel = ZServices::UIElementManager()->Dereference<ZUIText>(labelText_);
        textLabel->SetFontScale(labelFontSize_);
    }
}

void ZUILabeledElement::SetLabelTextAlignment(ZAlignment alignment)
{
    if (!labelText_.IsNull()) {
        ZUIText* textLabel = ZServices::UIElementManager()->Dereference<ZUIText>(labelText_);
        textLabel->SetHorizontalAlignment(alignment);
    }
}

void ZUILabeledElement::SetElement(const ZHUIElement& element)
{
    element_ = element;
    options = ZServices::UIElementManager()->Options(element);
    type = ZServices::UIElementManager()->Type(element);
    scene = ZServices::UIElementManager()->Scene(element);
}

void ZUILabeledElement::SetLabelPosition(Position labelPosition)
{
    labelPosition_ = labelPosition;
}

void ZUILabeledElement::SetLabelTextColor(const glm::vec4& color)
{
    labelTextColor_ = color;
    if (!labelText_.IsNull()) {
        ZServices::UIElementManager()->SetColor(labelText_, labelTextColor_);
    }
}

void ZUILabeledElement::SetLabelBackgroundColor(const glm::vec4& color)
{
    if (!background_.IsNull()) {
        ZServices::UIElementManager()->SetColor(background_, color);
    }
}

void ZUILabeledElement::CreateLabelField()
{
    auto sceneSP = scene.lock();
    if (!sceneSP) return;

    float labelWidth = label_.empty() ? 0.f : labelWidth_;

    ZUIElementOptions options;
    options.positioning = ZPositioning::Relative;
    options.scaling = ZPositioning::Relative;
    options.rect = ZRect(0.f, 0.f, labelWidth, 1.f);

    background_ = ZServices::UIElementManager()->Create(ZUIElementType::Panel, options, sceneSP);

    options.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    options.color = labelTextColor_;
    labelText_ = ZServices::UIElementManager()->Create(ZUIElementType::Text, options, sceneSP);
    ZUIText* textLabel = ZServices::UIElementManager()->Dereference<ZUIText>(labelText_);
    textLabel->SetText(label_);
    textLabel->SetFontScale(labelFontSize_);

    ZServices::UIElementManager()->AddChild(background_, labelText_);

    ZServices::UIElementManager()->AddChild(handle, background_);
}

void ZUILabeledElement::SetupElement()
{
    float labelWidth = label_.empty() ? 0.f : labelWidth_;

    ZServices::UIElementManager()->SetPositioning(element_, ZPositioning::Relative);
    ZServices::UIElementManager()->SetScaling(element_, ZPositioning::Relative);
    ZServices::UIElementManager()->SetRect(element_, ZRect(0.f, 0.f, 1.f - labelWidth, 1.f), options.calculatedRect);

    ZServices::UIElementManager()->AddChild(handle, element_);
}

ZHUIElement ZUILabeledElement::Create(const std::string& label, const ZHUIElement& element, Position labelPosition)
{
    ZHUIElement handle = ZServices::UIElementManager()->Create(ZUIElementType::LabeledElement);
    ZUILabeledElement* labeledElement = ZServices::UIElementManager()->Dereference<ZUILabeledElement>(handle);
    labeledElement->SetLabel(label);
    labeledElement->SetElement(element);
    labeledElement->SetLabelPosition(labelPosition);
    ZServices::UIElementManager()->Initialize(handle);
    return handle;
}