/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIListPanel.cpp

    Created by Adrian Sanchez on 17/03/2019.
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

#include "ZUIListPanel.hpp"
#include "ZServices.hpp"

ZUIListPanel::ZUIListPanel(const glm::vec2& position, const glm::vec2& scale) : ZUIElement(position, scale)
{
    type_ = ZUIElementType::ListPanel;
}

ZUIListPanel::ZUIListPanel(const ZUIElementOptions& options) : ZUIElement(options)
{
    type_ = ZUIElementType::ListPanel;
}

void ZUIListPanel::Initialize()
{
    ZUIElement::Initialize();
}

void ZUIListPanel::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    ZUIElement::Initialize(root);

    std::shared_ptr<ZOFObjectNode> node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (node == nullptr)
    {
        LOG("Could not initalize ZUIElement", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("itemHeight") != props.end() && props["itemHeight"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> itemHeightProp = props["itemHeight"]->Value<ZOFNumber>(0);
        itemHeight_ = itemHeightProp->value;
    }
}

void ZUIListPanel::AddChild(const std::shared_ptr<ZUIElement>& element)
{
    element->SetPosition(glm::vec2(Size().x, 2.f * itemHeight_ * children_.size() + itemHeight_));
    element->SetSize(glm::vec2(Size().x, itemHeight_));

    SetSize(glm::vec2(Size().x, glm::max(Size().y, itemHeight_ * children_.size())));

    ZUIElement::AddChild(element);
}

DEFINE_UI_CREATORS(ZUIListPanel)