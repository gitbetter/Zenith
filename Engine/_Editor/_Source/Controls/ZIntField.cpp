/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZIntField.hpp

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

#include "ZIntField.hpp"
#include "ZUIInputField.hpp"
#include "ZUIScrubber.hpp"
#include "ZUIHoverer.hpp"
#include "ZScene.hpp"
#include "ZDomain.hpp"
#include "ZUIText.hpp"
#include "ZServices.hpp"

void ZIntField::Initialize(const std::shared_ptr<ZScene>& scene)
{
    scrubber_ = std::make_shared<ZUIScrubber>();
    scrubber_->SetSensitivity(0.1f);
    hoverer_ = std::make_shared<ZUIHoverer>();
    inputField_->OnInputChanged([this](const std::string& newVal) {
        try {
            value_ = std::stoi(newVal);
        }
        catch (const std::exception& e) {
            value_ = -std::numeric_limits<int>::infinity();
        }
    });
}

void ZIntField::Update()
{
    auto labelRect = control_->LabelField()->CalculatedRect();
    auto elementRect = inputField_->CalculatedRect();

    if (scrubber_) {
        int scrubbedVal = scrubber_->Scrub<int>(labelRect);
        if (scrubbedVal != 0) {
            SetValue(lastValue_ + scrubbedVal);
        }
        else {
            lastValue_ = value_;
        }
    }

    if (hoverer_->Entered(labelRect)) {
        control_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::HorizontalResize));
    }
    else if (hoverer_->Entered(elementRect)) {
        control_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Caret));
    }
    else if (hoverer_->Exited(labelRect) || hoverer_->Exited(elementRect)) {
        control_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Arrow));
    }
}

void ZIntField::SetValue(const int& val)
{
    value_ = val;
    inputField_->SetText(std::to_string(value_));
}

std::shared_ptr<ZIntField> ZIntField::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto intField = std::make_shared<ZIntField>(theme);

    intField->inputField_ = ZUIInputField::Create(options, scene);
    intField->inputField_->SetCharacterFilter([](char c) { return std::isdigit(c) || c == '-'; });
    intField->inputField_->SetHighlightBorder(ZUIBorder(theme.highlightColor, 1.f, 0.f));
    intField->control_ = ZUILabeledElement::Create(label, intField->inputField_);
    intField->Initialize(scene);

    return intField;
}
