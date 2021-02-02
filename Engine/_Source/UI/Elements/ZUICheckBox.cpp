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
#include "ZShader.hpp"
#include "ZUIImage.hpp"
#include "ZFireEvent.hpp"

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

    checkImage_ = std::make_shared<ZUIImage>("/Textures/UI/checkmark.png", glm::vec2(0.f) + Size(), Size());
    checkImage_->SetScene(Scene());
    checkImage_->Initialize();
    AddChild(checkImage_);

    ZServices::EventAgent()->Subscribe(this, &ZUICheckBox::HandleMousePress);
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

void ZUICheckBox::CleanUp()
{
    ZUIElement::CleanUp();
    ZServices::EventAgent()->Unsubscribe(this, &ZUICheckBox::HandleMousePress);
}

void ZUICheckBox::HandleMousePress(const std::shared_ptr<ZFireEvent>& event)
{
    if (event->Done() && TrySelect(glm::vec3(event->X(), event->Y(), event->Z())))
    {
        checked_ = !checked_;
        if (checked_)
        {
            checkImage_->Show();
        }
        else
        {
            checkImage_->Hide();
        }
    }
}

std::shared_ptr<ZUICheckBox> ZUICheckBox::Create()
{
    std::shared_ptr<ZUICheckBox> element = std::make_shared<ZUICheckBox>();
    return element;
}

std::shared_ptr<ZUICheckBox> ZUICheckBox::Create(const ZUIElementOptions& options)
{
    std::shared_ptr<ZUICheckBox> element = std::make_shared<ZUICheckBox>(options);
    return element;
}

std::shared_ptr<ZUICheckBox> ZUICheckBox::CreateIn(const std::shared_ptr<ZScene>& scene, const ZUIElementOptions& options)
{
    auto element = ZUICheckBox::Create(options);
    element->SetScene(scene);
    element->Initialize();
    return element;
}
