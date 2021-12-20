/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZDropMenu.cpp

    Created by Adrian Sanchez on 11/07/21.
    Copyright © 2021 Pervasive Sense. All rights reserved.
 
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

#include "ZDropMenu.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZUIText.hpp"
#include "ZUIPanel.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZDropMenuShowEvent.hpp"
#include "ZScene.hpp"

void ZDropMenu::Initialize(const std::shared_ptr<ZScene>& scene)
{
    scene_ = scene;

    ZAssets::UIElementManager()->Hide(menu_);

    ZServices::EventAgent()->Subscribe(this, &ZDropMenu::HandleDropMenuShownEvent);
}

void ZDropMenu::Update()
{
    auto clickBounds = ZAssets::UIElementManager()->CalculatedRect(activationElement_);
    if (clicker_.Click(clickBounds))
    {
        if (ZAssets::UIElementManager()->Hidden(menu_))
        {
            ZAssets::UIElementManager()->Show(menu_);
            ZServices::EventAgent()->Trigger(std::make_shared<ZDropMenuShowEvent>(menu_));
        }
        else
        {
            ZAssets::UIElementManager()->Hide(menu_);
        }
    }

    for (auto const& [key, val] : menuItemCallbacks_)
    {
        const ZHUIElement& itemHandle = ZHUIElement(key);
        auto itemBounds = ZAssets::UIElementManager()->CalculatedRect(itemHandle);
	    if (hoverer_.Hover(itemBounds))
	    {
		    ZAssets::UIElementManager()->SetColor(itemHandle, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	    }
	    else
	    {
		    ZAssets::UIElementManager()->SetColor(itemHandle, glm::vec4(0.0f));
	    }

        if (menuItemClickers_[key].Click(itemBounds))
        {
            val();
        }
    }
}

void ZDropMenu::AddMenuItem(const std::string& label, const std::function<void()>& callback)
{
    ZUIElementOptions options;
    options.positioning = ZPositioning::Relative;
    options.scaling = ZPositioning::Relative;
    options.rect = ZRect(0.0f, 0.0f, 1.0f, 1.0f);
    ZHUIElement menuItem = ZAssets::UIElementManager()->Create(ZUIElementType::Panel, options, ZHUIElement(), scene_);

    options = ZUIElementOptions();
    options.positioning = ZPositioning::Relative;
    options.scaling = ZPositioning::Relative;
    options.rect = ZRect(0.025f, 0.0f, 0.95f, 1.0f);
    options.color = glm::vec4(1.0f);
    ZHUIElement menuItemText = ZAssets::UIElementManager()->Create(ZUIElementType::Text, options, ZHUIElement(), scene_);

    ZAssets::UIElementManager()->Dereference<ZUIText>(menuItemText)->SetText(label);
    ZAssets::UIElementManager()->Dereference<ZUIText>(menuItemText)->SetHorizontalAlignment(ZAlignment::Left);
    ZAssets::UIElementManager()->Dereference<ZUIText>(menuItemText)->SetVerticalAlignment(ZAlignment::Middle);

    ZAssets::UIElementManager()->AddChild(menuItem, menuItemText);
    ZAssets::UIElementManager()->AddChild(menu_, menuItem);

    menuItemCallbacks_[menuItem.Handle()] = callback;
    menuItemClickers_[menuItem.Handle()] = ZUIClicker();
}

std::shared_ptr<ZDropMenu> ZDropMenu::Create(const ZHUIElement& activationElement, const ZUIElementOptions& elementOptions, const std::shared_ptr<ZScene>& scene /*= nullptr*/, ZUITheme theme /*= ZUITheme()*/)
{
    std::shared_ptr<ZDropMenu> dropMenu = std::make_shared<ZDropMenu>(theme);
    dropMenu->activationElement_ = activationElement;
    dropMenu->menu_ = ZAssets::UIElementManager()->Create(ZUIElementType::ListPanel, elementOptions, ZHUIElement(), scene);

    dropMenu->Initialize(scene);

    ZAssets::UIElementManager()->AddChild(activationElement, dropMenu->menu_);

    return dropMenu;
}

void ZDropMenu::HandleDropMenuShownEvent(const std::shared_ptr<class ZDropMenuShowEvent>& event)
{
    if (menu_ != event->DropMenu())
    {
        ZAssets::UIElementManager()->Hide(menu_);
    }
}
