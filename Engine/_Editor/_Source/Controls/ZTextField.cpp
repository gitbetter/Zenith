/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZTextField.cpp

    Created by Adrian Sanchez on 02/15/21.
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

#include "ZTextField.hpp"
#include "ZUIInputField.hpp"
#include "ZUIHoverer.hpp"
#include "ZScene.hpp"
#include "ZDomain.hpp"

void ZTextField::Initialize(const std::shared_ptr<ZScene>& scene)
{
    hoverer_ = std::make_shared<ZUIHoverer>();
}

void ZTextField::Update()
{
    auto elementRect = inputField_->CalculatedRect();

    if (hoverer_->Entered(elementRect)) {
        control_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Caret));
    }
    else if (hoverer_->Exited(elementRect)) {
        control_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Arrow));
    }
}

void ZTextField::SetValue(const std::string& val)
{
    if (val == lastValue_) return;
    lastValue_ = value_;
    value_ = val;
    inputField_->SetText(val);
}

std::shared_ptr<ZTextField> ZTextField::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto textField = std::make_shared<ZTextField>(theme);

    textField->inputField_ = ZUIInputField::Create(options, scene);
    textField->inputField_->SetCharacterFilter([](char c) { return true; });
    textField->inputField_->SetHighlightBorder(ZUIBorder(theme.highlightColor, 1.f, 0.f));
    textField->control_ = ZUILabeledElement::Create(label, textField->inputField_);
    textField->Initialize(scene);

    return textField;
}
