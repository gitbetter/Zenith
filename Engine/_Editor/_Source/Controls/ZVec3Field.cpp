/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZVec3Field.hpp

    Created by Adrian Sanchez on 02/17/21.
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

#include "ZVec3Field.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZFloatField.hpp"
#include "ZUIScrubber.hpp"
#include "ZUIText.hpp"
#include "ZUIPanel.hpp"
#include "ZUIHorizontalLayout.hpp"

void ZVec3Field::Update()
{
    xInputField_->Update();
    yInputField_->Update();
    zInputField_->Update();
    xInputField_->Value(value_.x); yInputField_->Value(value_.y); zInputField_->Value(value_.z);
}

void ZVec3Field::SetValue(const glm::vec3& val)
{
    if (val == lastValue_) return;
    lastValue_ = value_;
    value_ = val;
    xInputField_->SetValue(val.x);
    yInputField_->SetValue(val.y);
    zInputField_->SetValue(val.z);
}

std::shared_ptr<ZVec3Field> ZVec3Field::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto vec3Field = std::make_shared<ZVec3Field>(theme);

    ZUIElementOptions fieldOptions = options;
    fieldOptions.color = glm::vec4(0.f);
    ZUILayoutOptions layoutOptions;
    layoutOptions.verticalAlign = ZAlignment::Middle;
    fieldOptions.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);

    auto container = ZAssets::UIElementManager()->Create(ZUIElementType::Panel, fieldOptions, ZHUIElement(), scene);

    fieldOptions = ZUIElementOptions();
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.f, 0.f, 0.33f, 1.f);
    fieldOptions.color = options.color;

    vec3Field->xInputField_ = ZFloatField::Create("X", fieldOptions, scene, theme);
    auto labeledElementObj = ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(vec3Field->xInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
    ZAssets::UIElementManager()->AddChild(container, vec3Field->xInputField_->Control());

    vec3Field->yInputField_ = ZFloatField::Create("Y", fieldOptions, scene, theme);
    labeledElementObj = ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(vec3Field->yInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(0.f, 1.f, 0.f, 1.f));
    ZAssets::UIElementManager()->AddChild(container, vec3Field->yInputField_->Control());

    vec3Field->zInputField_ = ZFloatField::Create("Z", fieldOptions, scene, theme);
    labeledElementObj = ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(vec3Field->zInputField_->Control());
    labeledElementObj->SetLabelTextAlignment(ZAlignment::Middle);
    labeledElementObj->SetLabelFontSize(13.f);
    labeledElementObj->SetLabelBackgroundColor(glm::vec4(0.f, 0.f, 1.f, 1.f));
    ZAssets::UIElementManager()->AddChild(container, vec3Field->zInputField_->Control());

    vec3Field->control_ = ZUILabeledElement::Create(label, container);

    vec3Field->Initialize(scene);

    return vec3Field;
}
