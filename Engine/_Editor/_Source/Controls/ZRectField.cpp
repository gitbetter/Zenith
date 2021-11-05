/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZRectField.hpp

    Created by Adrian Sanchez on 02/18/21.
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

#include "ZRectField.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZFloatField.hpp"
#include "ZUIScrubber.hpp"
#include "ZUIText.hpp"
#include "ZUIPanel.hpp"
#include "ZUIHorizontalLayout.hpp"

void ZRectField::Update()
{
    xInputField_->Update();
    yInputField_->Update();
    wInputField_->Update();
    hInputField_->Update();
    xInputField_->Value(value_.position.x);
    yInputField_->Value(value_.position.y);
    wInputField_->Value(value_.size.x);
    hInputField_->Value(value_.size.y);
}

void ZRectField::SetValue(const ZRect& val)
{
    if (val == lastValue_) return;
    lastValue_ = value_;
    value_ = val;
    xInputField_->SetValue(val.position.x);
    yInputField_->SetValue(val.position.y);
    wInputField_->SetValue(val.size.x);
    hInputField_->SetValue(val.size.y);
}

std::shared_ptr<ZRectField> ZRectField::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto rectField = std::make_shared<ZRectField>(theme);

    ZUIElementOptions fieldOptions = options;
    fieldOptions.color = glm::vec4(0.f);
    ZUILayoutOptions layoutOptions;
    layoutOptions.verticalAlign = ZAlignment::Middle;
    fieldOptions.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);

    auto container = ZAssets::UIElementManager()->Create(ZUIElementType::Panel, fieldOptions, ZHUIElement(), scene);

    fieldOptions = ZUIElementOptions();
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.f, 0.f, 0.25f, 1.f);
    fieldOptions.color = options.color;

    rectField->xInputField_ = ZFloatField::Create("X", fieldOptions, scene, theme);
    auto labeledElementObj = ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(rectField->xInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
    ZAssets::UIElementManager()->AddChild(container, rectField->xInputField_->Control());

    rectField->yInputField_ = ZFloatField::Create("Y", fieldOptions, scene, theme);
    labeledElementObj = ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(rectField->yInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
    ZAssets::UIElementManager()->AddChild(container, rectField->yInputField_->Control());

    rectField->wInputField_ = ZFloatField::Create("W", fieldOptions, scene, theme);
    labeledElementObj = ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(rectField->wInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
    ZAssets::UIElementManager()->AddChild(container, rectField->wInputField_->Control());

    rectField->hInputField_ = ZFloatField::Create("H", fieldOptions, scene, theme);
    labeledElementObj = ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(rectField->hInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
    ZAssets::UIElementManager()->AddChild(container, rectField->hInputField_->Control());

    rectField->control_ = ZUILabeledElement::Create(label, container);

    rectField->Initialize(scene);

    return rectField;
}
