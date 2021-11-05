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
#include "ZAssets.hpp"

ZUIListPanel::ZUIListPanel()
{
    type = ZUIElementType::ListPanel;
}

void ZUIListPanel::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    ZOFPropertyMap& props = dataNode->properties;

    if (props.find("itemHeight") != props.end() && props["itemHeight"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> itemHeightProp = props["itemHeight"]->Value<ZOFNumber>(0);
        itemHeight_ = itemHeightProp->value;
    }
}

void ZUIListPanel::OnChildAdded(const ZHUIElement& element)
{
    const glm::vec2 thisSize = ZAssets::UIElementManager()->Size(handle);
    const size_t thisNumChildren = ZAssets::UIElementManager()->Children(handle).size();

    ZAssets::UIElementManager()->SetPosition(element, glm::vec2(thisSize.x, 2.f * itemHeight_ * thisNumChildren + itemHeight_));
    ZAssets::UIElementManager()->SetSize(element, glm::vec2(thisSize.x, itemHeight_));

    ZAssets::UIElementManager()->SetSize(handle, glm::vec2(thisSize.x, glm::max(thisSize.y, itemHeight_ * thisNumChildren)));
}