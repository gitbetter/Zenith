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

ZUICheckBox::ZUICheckBox(const glm::vec2& position, const glm::vec2& scale) : ZUIElement(position, scale)
{
    type_ = ZUIElementType::CheckBox;
}

ZUICheckBox::ZUICheckBox(const ZUIElementOptions& options) : ZUIElement(options)
{
    type_ = ZUIElementType::CheckBox;
}

void ZUICheckBox::Initialize() {
    ZUIElement::Initialize();

    auto scene = Scene();
    if (!scene) {
        LOG("Cannot initialize a ZUICheckbox that is not attached to a scene", ZSeverity::Error);
    }

    ZUIElementOptions options;
    options.positioning = ZPositioning::Relative;
    options.scaling = ZPositioning::Relative;
    options.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    options.color = glm::vec4(1.f);
    options.flipped = true;
    options.hidden = !checked_;
    checkImage_ = ZUIImage::Create(options, Scene());
    checkImage_->SetImage("/Textures/UI/checkmark.png");
    AddChild(checkImage_);
}

void ZUICheckBox::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    ZUIElement::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (node == nullptr)
    {
        LOG("Could not initalize ZUICheckbox", ZSeverity::Error); return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("checkColor") != props.end() && props["checkColor"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> chkColorProp = props["checkColor"]->Value<ZOFNumberList>(0);
        checkImage_->SetColor(glm::vec4(chkColorProp->value[0], chkColorProp->value[1], chkColorProp->value[2], chkColorProp->value[3]));
    }
}

void ZUICheckBox::SetChecked(bool checked)
{
    checked_ = checked;
    if (checked_)
    {
        checkImage_->Show();
    }
    else
    {
        checkImage_->Hide();
    }
}

void ZUICheckBox::Toggle()
{
    SetChecked(!checked_);
}

DEFINE_UI_CREATORS(ZUICheckBox)
