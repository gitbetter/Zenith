/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZMenuBar.cpp
 
    Created by Adrian Sanchez on 23/05/19.
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

#include "ZMenuBar.hpp"
#include "ZAssets.hpp"
#include "ZUIPanel.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZUIText.hpp"
#include "ZServices.hpp"

void ZMenuBar::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorTool::Initialize(scene);

    ZAssets::UIElementManager()->SetColor(container_, theme_.secondaryColor);
    ZAssets::UIElementManager()->SetRect(container_, ZRect(0.f, 0.f, 1.f, 0.35f));

	SetupMenuLayout(scene);

	AddMenuOption("New");
	AddMenuOption("Edit");
	AddMenuOption("Help");
}

void ZMenuBar::Update()
{
	for (const ZHUIElement& handle : menuItems_)
	{
		ZRect handleRect = ZAssets::UIElementManager()->CalculatedRect(handle);
		if (clicker_.Click(handleRect))
		{
			ILOG("Yo!");
		}

		if (hoverer_.Hover(handleRect))
		{
			ZAssets::UIElementManager()->SetColor(handle, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
		}
		else
		{
			ZAssets::UIElementManager()->SetColor(handle, glm::vec4(0.0f));
		}
	}
}

void ZMenuBar::AddMenuOption(const std::string& label)
{
	ZUIElementOptions panelElementOptions;
	panelElementOptions.positioning = ZPositioning::Relative;
	panelElementOptions.scaling = ZPositioning::Relative;
	panelElementOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
	panelElementOptions.maxSize = glm::vec2(35.0f, 0.0f);
	ZHUIElement menuOptionPanel = ZAssets::UIElementManager()->Create(ZUIElementType::Panel, panelElementOptions, ZHUIElement(), ZAssets::UIElementManager()->Scene(container_));

	ZUIElementOptions textElementOptions;
	textElementOptions.positioning = ZPositioning::Relative;
	textElementOptions.scaling = ZPositioning::Relative;
	textElementOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
	textElementOptions.color = glm::vec4(1.f);
	ZHUIElement menuOption = ZAssets::UIElementManager()->Create(ZUIElementType::Text, textElementOptions, ZHUIElement(), ZAssets::UIElementManager()->Scene(container_));

	ZUIText* textElement = ZAssets::UIElementManager()->Dereference<ZUIText>(menuOption);
	textElement->SetText(label);
	textElement->SetFontScale(14.0f);
	textElement->SetVerticalAlignment(ZAlignment::Middle);
	textElement->SetHorizontalAlignment(ZAlignment::Middle);

    ZAssets::UIElementManager()->AddChild(menuOptionPanel, menuOption);
	ZAssets::UIElementManager()->AddChild(menuLayoutPanel_, menuOptionPanel);

	menuItems_.push_back(menuOptionPanel);
}

void ZMenuBar::SetupMenuLayout(const std::shared_ptr<ZScene>& scene)
{
	ZUIElementOptions horizontalPanelOptions;
	horizontalPanelOptions.positioning = ZPositioning::Relative;
	horizontalPanelOptions.scaling = ZPositioning::Relative;
	horizontalPanelOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
	horizontalPanelOptions.padding = glm::vec2(10.0f, 0.0f);
	ZUILayoutOptions horizontalLayoutOptions;
	horizontalLayoutOptions.verticalAlign = ZAlignment::Middle;
	horizontalLayoutOptions.itemSpacing = 10.0f;
	horizontalPanelOptions.layout = std::make_shared<ZUIHorizontalLayout>(horizontalLayoutOptions);
	menuLayoutPanel_ = ZAssets::UIElementManager()->Create(ZUIElementType::Panel, horizontalPanelOptions, ZHUIElement(), scene);

	ZAssets::UIElementManager()->AddChild(container_, menuLayoutPanel_);
}
