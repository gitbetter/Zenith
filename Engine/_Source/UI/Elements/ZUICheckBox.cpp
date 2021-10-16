/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUICheckBox.cpp

    Created by Adrian Sanchez on 16/03/2019.
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

#include "ZServices.hpp"
#include "ZUICheckBox.hpp"
#include "ZUIImage.hpp"
#include "ZUIClicker.hpp"
#include "ZInputButtonEvent.hpp"

ZUICheckBox::ZUICheckBox() : ZUIElement()
{
    type = ZUIElementType::CheckBox;
}

void ZUICheckBox::OnInitialize() {
    ZUIElementOptions options;
    options.positioning = ZPositioning::Relative;
    options.scaling = ZPositioning::Relative;
    options.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    options.color = glm::vec4(1.f);
    options.flipped = true;
    options.hidden = !checked_;
    checkImage_ = ZServices::UIElementManager()->Create(ZUIElementType::Image, options, ZHUIElement(), ZServices::UIElementManager()->Scene(handle));
    ZUIImage* image = ZServices::UIElementManager()->Dereference<ZUIImage>(checkImage_);
    image->SetImage("/Textures/UI/checkmark.png");
    ZServices::UIElementManager()->AddChild(handle, checkImage_);
}

void ZUICheckBox::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    ZOFPropertyMap& props = dataNode->properties;

    if (props.find("checkColor") != props.end() && props["checkColor"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> chkColorProp = props["checkColor"]->Value<ZOFNumberList>(0);
        ZServices::UIElementManager()->SetColor(checkImage_, glm::vec4(chkColorProp->value[0], chkColorProp->value[1], chkColorProp->value[2], chkColorProp->value[3]));
    }
}

void ZUICheckBox::SetChecked(bool checked)
{
    checked_ = checked;
    if (checked_)
    {
        ZServices::UIElementManager()->Show(handle);
    }
    else
    {
        ZServices::UIElementManager()->Hide(handle);
    }
}

void ZUICheckBox::Toggle()
{
    SetChecked(!checked_);
}
