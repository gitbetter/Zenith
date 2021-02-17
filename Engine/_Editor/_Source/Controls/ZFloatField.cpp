/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZFloatField.hpp

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

#include "ZFloatField.hpp"
#include "ZUIInputField.hpp"
#include "ZUIScrubber.hpp"
#include "ZUIText.hpp"
#include "ZServices.hpp"
#include "ZWindowResizeEvent.hpp"

void ZFloatField::Initialize(const std::shared_ptr<ZScene>& scene)
{
    scrubber_ = std::make_shared<ZUIScrubber>();
    scrubber_->SetSensitivity(0.001f);
    control_->OnInputChanged([this](const std::string& newVal) {
        try {
            value_ = std::stof(newVal);
        }
        catch (const std::exception& e) {
            value_ = -std::numeric_limits<float>::infinity();
        }
    });
}

void ZFloatField::Update()
{
    if (scrubber_) {
        float scrubbedVal = scrubber_->Scrub<float>(control_->LabelField()->CalculatedRect());
        if (scrubbedVal != 0) {
            SetValue(lastValue_ + scrubbedVal);
        }
        else {
            lastValue_ = value_;
        }
    }
}

void ZFloatField::SetValue(const float& val)
{
    value_ = val;
    control_->SetText(std::to_string(value_));
}

std::shared_ptr<ZFloatField> ZFloatField::Create(const std::string& label, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto intField = std::make_shared<ZFloatField>(theme);

    ZUIElementOptions fieldOptions;
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.05f, 0.05f, 0.9f, 0.05f);
    fieldOptions.maxSize = glm::vec2(0.f, 30.f);
    intField->control_ = ZUIInputField::Create(fieldOptions, scene);
    intField->control_->SetLabel(label);
    intField->control_->SetCharacterFilter([](char c) { return std::isdigit(c) || c == '.' || c == '-'; });
    intField->control_->SetHighlightBorder(ZUIBorder(theme.highlightColor, 1.f, 0.f));
    intField->Initialize(scene);

    return intField;
}
