/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZBoolField.hpp

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

#include "ZBoolField.hpp"
#include "ZServices.hpp"
#include "ZUIText.hpp"
#include "ZUICheckBox.hpp"
#include "ZUIPanel.hpp"
#include "ZUIHoverer.hpp"
#include "ZUIClicker.hpp"
#include "ZScene.hpp"
#include "ZDomain.hpp"

void ZBoolField::Initialize(const std::shared_ptr<ZScene>& scene)
{
    hoverer_ = std::make_shared<ZUIHoverer>();
    clicker_ = std::make_shared<ZUIClicker>();
}

void ZBoolField::Update()
{
    auto elementRect = checkbox_->CalculatedRect();

    if (hoverer_->Entered(elementRect)) {
        control_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Hand));
    }
    else if (hoverer_->Exited(elementRect)) {
        control_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Arrow));
    }

    if (clicker_->Click(elementRect)) {
        checkbox_->Toggle();
    }

    value_ = checkbox_->Checked();
}

void ZBoolField::SetValue(const bool& val)
{
    value_ = val;
    checkbox_->SetChecked(val);
}

std::shared_ptr<ZBoolField> ZBoolField::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto boolField = std::make_shared<ZBoolField>(theme);

    ZUIElementOptions checkboxOptions = options;
    checkboxOptions.maxSize = glm::vec2(100.f, 0.f);
    boolField->checkbox_ = ZUICheckBox::Create(checkboxOptions, scene);

    boolField->control_ = ZUILabeledElement::Create(label, boolField->checkbox_);

    boolField->Initialize(scene);

    return boolField;
}
