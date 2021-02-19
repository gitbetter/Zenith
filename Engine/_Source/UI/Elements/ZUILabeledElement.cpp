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

ZUILabeledElement::ZUILabeledElement(const std::string& label, const std::shared_ptr<ZUIElement>& element)
    : ZUIElement()
{
    // A labeled element is just a wrapper/decorator object, so it should inherit most of the
    // properties of the wrapped element
    element_ = element;
    label_ = label;
    options_ = element_->Options();
    type_ = element->Type();
    scene_ = element->Scene();
    options_.color = glm::vec4(0.f);
}

void ZUILabeledElement::Initialize()
{
    ZUIElement::Initialize();

    auto scene = Scene();
    if (!scene)
    {
        LOG("ZUILabeledElement was not attached to scene during initialization", ZSeverity::Error);
        return;
    }

    ZUILayoutOptions layoutOptions;
    layoutOptions.verticalAlign = ZAlignment::Middle;
    layoutOptions.dimensions = options_.calculatedRect;
    options_.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);

    CreateLabelField();
    SetupElement();
}

void ZUILabeledElement::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    ZUIElement::Initialize(root);
}

void ZUILabeledElement::SetLabel(const std::string& label)
{
    labelText_->SetText(label);
    float labelWidth = label.empty() ? 0.f : labelWidth_;
    labelText_->SetRect(ZRect(0.f, 0.f, labelWidth, 1.f), options_.calculatedRect);
    element_->SetRect(ZRect(0.f, 0.f, 1.f - labelWidth, 1.f), options_.calculatedRect);
}

void ZUILabeledElement::SetLabelWidth(float width)
{
    labelWidth_ = width;
    if (labelText_) {
        labelText_->SetRect(ZRect(0.f, 0.f, labelWidth_, 1.f), options_.calculatedRect);
        element_->SetRect(ZRect(0.f, 0.f, 1.f - labelWidth_, 1.f), options_.calculatedRect);
    }
}

void ZUILabeledElement::SetLabelFontSize(float size)
{
    labelFontSize_ = size;
    if (labelText_) {
        labelText_->SetFontScale(labelFontSize_);
    }
}

void ZUILabeledElement::SetLabelTextAlignment(ZAlignment alignment)
{
    if (labelText_) {
        labelText_->SetAlignment(alignment);
    }
}

void ZUILabeledElement::SetLabelTextColor(const glm::vec4& color)
{
    labelTextColor_ = color;
    if (labelText_) {
        labelText_->SetColor(labelTextColor_);
    }
}

void ZUILabeledElement::CreateLabelField()
{
    auto scene = Scene();
    if (!scene) return;

    float labelWidth = label_.empty() ? 0.f : labelWidth_;

    ZUIElementOptions textOptions;
    textOptions.positioning = ZPositioning::Relative;
    textOptions.scaling = ZPositioning::Relative;
    textOptions.rect = ZRect(0.f, 0.f, labelWidth, 1.f);
    textOptions.color = labelTextColor_;
    labelText_ = ZUIText::Create(textOptions, scene);
    labelText_->SetText(label_);
    labelText_->SetFontScale(labelFontSize_);

    AddChild(labelText_);
}

void ZUILabeledElement::SetupElement()
{
    float labelWidth = label_.empty() ? 0.f : labelWidth_;

    element_->SetPositioning(ZPositioning::Relative);
    element_->SetScaling(ZPositioning::Relative);
    element_->SetRect(ZRect(0.f, 0.f, 1.f - labelWidth, 1.f), options_.calculatedRect);

    AddChild(element_);
}

std::shared_ptr<ZUILabeledElement> ZUILabeledElement::Create(const std::string& label, const std::shared_ptr<ZUIElement>& element)
{
    auto labeledElement = std::make_shared<ZUILabeledElement>(label, element);
    labeledElement->Initialize();
    return labeledElement;
}