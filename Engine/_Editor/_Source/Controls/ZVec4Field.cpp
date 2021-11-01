/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZVec4Field.hpp

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

#include "ZVec4Field.hpp"
#include "ZServices.hpp"
#include "ZFloatField.hpp"
#include "ZUIScrubber.hpp"
#include "ZUIText.hpp"
#include "ZUIPanel.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZScene.hpp"

void ZVec4Field::Update()
{
    xInputField_->Update();
    yInputField_->Update();
    zInputField_->Update();
    wInputField_->Update();
    xInputField_->Value(value_.x); yInputField_->Value(value_.y); zInputField_->Value(value_.z); wInputField_->Value(value_.w);
}

void ZVec4Field::SetValue(const glm::vec4& val)
{
    if (val == lastValue_)
    {
        return;
    }

    lastValue_ = value_;
    value_ = val;
    xInputField_->SetValue(val.x);
    yInputField_->SetValue(val.y);
    zInputField_->SetValue(val.z);
    wInputField_->SetValue(val.w);
}

std::shared_ptr<ZVec4Field> ZVec4Field::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto vec4Field = std::make_shared<ZVec4Field>(theme);

    ZUIElementOptions fieldOptions = options;
    fieldOptions.color = glm::vec4(0.f);
    ZUILayoutOptions layoutOptions;
    layoutOptions.verticalAlign = ZAlignment::Middle;
    fieldOptions.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);

    auto container = ZServices::UIElementManager()->Create(ZUIElementType::Panel, fieldOptions, ZHUIElement(), scene);

    fieldOptions = ZUIElementOptions();
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.f, 0.f, 0.25f, 1.f);
    fieldOptions.color = options.color;

    vec4Field->xInputField_ = ZFloatField::Create("X", fieldOptions, scene, theme);
    auto labeledElementObj = ZServices::UIElementManager()->Dereference<ZUILabeledElement>(vec4Field->xInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
    ZServices::UIElementManager()->AddChild(container, vec4Field->xInputField_->Control());

    vec4Field->yInputField_ = ZFloatField::Create("Y", fieldOptions, scene, theme);
    labeledElementObj = ZServices::UIElementManager()->Dereference<ZUILabeledElement>(vec4Field->yInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
    ZServices::UIElementManager()->AddChild(container, vec4Field->yInputField_->Control());

    vec4Field->zInputField_ = ZFloatField::Create("Z", fieldOptions, scene, theme);
    labeledElementObj = ZServices::UIElementManager()->Dereference<ZUILabeledElement>(vec4Field->zInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(0.f, 0.f, 1.f, 1.f));
    ZServices::UIElementManager()->AddChild(container, vec4Field->zInputField_->Control());

    vec4Field->wInputField_ = ZFloatField::Create("W", fieldOptions, scene, theme);
    labeledElementObj = ZServices::UIElementManager()->Dereference<ZUILabeledElement>(vec4Field->wInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(0.f, 0.f, 0.f, 1.f));
    ZServices::UIElementManager()->AddChild(container, vec4Field->wInputField_->Control());

    vec4Field->control_ = ZUILabeledElement::Create(label, container);

    vec4Field->Initialize(scene);

    return vec4Field;
}
