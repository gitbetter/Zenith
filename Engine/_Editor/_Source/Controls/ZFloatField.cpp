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
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZUIInputField.hpp"
#include "ZUIText.hpp"
#include "ZScene.hpp"
#include "ZDomain.hpp"
#include "ZWindowResizeEvent.hpp"

void ZFloatField::Initialize(const std::shared_ptr<ZScene>& scene)
{
    scrubber_.SetSensitivity(0.001f);

    ZAssets::UIElementManager()->Dereference<ZUIInputField>(inputField_)->OnInputChanged([this](const std::string& newVal)
        {
            try
            {
                value_ = std::stof(newVal);
            }
            catch (const std::exception& e)
            {
                value_ = -std::numeric_limits<float>::infinity();
            }
        });
}

void ZFloatField::Update()
{
    auto labelRect = ZAssets::UIElementManager()->CalculatedRect(ZAssets::UIElementManager()->Dereference<ZUILabeledElement>(control_)->LabelField());
    auto elementRect = ZAssets::UIElementManager()->CalculatedRect(inputField_);

    float scrubbedVal = scrubber_.Scrub<float>(labelRect);
    if (scrubbedVal != 0)
    {
        SetValue(lastScrubbedValue_ + scrubbedVal);
    }
    else
    {
        lastScrubbedValue_ = value_;
    }

    if (hoverer_.Entered(labelRect))
    {
        ZAssets::UIElementManager()->Scene(control_)->Domain()->SetCursor(ZCursor(ZSystemCursorType::HorizontalResize));
    }
    else if (hoverer_.Entered(elementRect))
    {
        ZAssets::UIElementManager()->Scene(control_)->Domain()->SetCursor(ZCursor(ZSystemCursorType::Caret));
    }
    else if (hoverer_.Exited(labelRect) || hoverer_.Exited(elementRect))
    {
        ZAssets::UIElementManager()->Scene(control_)->Domain()->SetCursor(ZCursor(ZSystemCursorType::Arrow));
    }
}

void ZFloatField::SetValue(const float& val)
{
    if (val == lastValue_)
    {
        return;
    }

    lastValue_ = value_;
    value_ = val;

    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << value_;

    ZAssets::UIElementManager()->Dereference<ZUIInputField>(inputField_)->SetText(stream.str());
}

std::shared_ptr<ZFloatField> ZFloatField::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto floatField = std::make_shared<ZFloatField>(theme);

    floatField->inputField_ = ZAssets::UIElementManager()->Create(ZUIElementType::InputField, options, ZHUIElement(), scene);
    auto inputFieldObj = ZAssets::UIElementManager()->Dereference<ZUIInputField>(floatField->inputField_);
    inputFieldObj->SetCharacterFilter([](char c) { return std::isdigit(c) || c == '.' || c == '-'; });
    inputFieldObj->SetHighlightBorder(ZUIBorder(theme.highlightColor, 1.f, 0.f));

    floatField->control_ = ZUILabeledElement::Create(label, floatField->inputField_);

    floatField->Initialize(scene);

    return floatField;
}
