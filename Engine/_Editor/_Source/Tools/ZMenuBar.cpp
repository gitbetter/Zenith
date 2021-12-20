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
#include "ZDropMenu.hpp"
#include "ZServices.hpp"

void ZMenuBar::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorTool::Initialize(scene);

    ZAssets::UIElementManager()->SetColor(container_, theme_.secondaryColor);
    ZAssets::UIElementManager()->SetRect(container_, ZRect(0.f, 0.f, 1.f, 0.35f));

	SetupMenuLayout(scene);

	AddMenuOption("New", {
		{"Project", []() { ILOG("New Project!"); }},
		{"Scene", []() { ILOG("New Scene!"); }}
	});
	AddMenuOption("Edit", {});
	AddMenuOption("Help", {});
}

void ZMenuBar::Update()
{
	for (int i = 0; i < menuItems_.size(); i++)
	{
		const ZHUIElement& handle = menuItems_[i];
		ZRect handleRect = ZAssets::UIElementManager()->CalculatedRect(handle);

		if (hoverer_.Hover(handleRect))
		{
			ZAssets::UIElementManager()->SetColor(handle, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
		}
		else
		{
			ZAssets::UIElementManager()->SetColor(handle, glm::vec4(0.0f));
		}

		if (itemDropMenus_.find(handle.Handle()) != itemDropMenus_.end())
		{
			itemDropMenus_[handle.Handle()]->Update();
		}
	}
}

void ZMenuBar::AddMenuOption(const std::string& label, const std::initializer_list<std::pair<std::string, std::function<void()>>>& suboptions)
{
	if (std::find(menuItemLabels_.begin(), menuItemLabels_.end(), label) == menuItemLabels_.end())
	{
		auto containerScene = ZAssets::UIElementManager()->Scene(container_);

		ZUIElementOptions options;
		options.positioning = ZPositioning::Relative;
		options.scaling = ZPositioning::Relative;
		options.rect = ZRect(0.f, 0.f, 1.f, 1.f);
		options.maxSize = glm::vec2(35.0f, 0.0f);
		ZHUIElement menuItemPanel = ZAssets::UIElementManager()->Create(ZUIElementType::Panel, options, ZHUIElement(), containerScene);

		options.maxSize = glm::vec2(0.0f);
		options.color = glm::vec4(1.0f);
		ZHUIElement menuItemText = ZAssets::UIElementManager()->Create(ZUIElementType::Text, options, ZHUIElement(), containerScene);

		ZUIText* textElement = ZAssets::UIElementManager()->Dereference<ZUIText>(menuItemText);
		textElement->SetText(label);
		textElement->SetVerticalAlignment(ZAlignment::Middle);
		textElement->SetHorizontalAlignment(ZAlignment::Middle);

		ZAssets::UIElementManager()->AddChild(menuItemPanel, menuItemText);
		ZAssets::UIElementManager()->AddChild(menuLayoutPanel_, menuItemPanel);

		ZUIElementOptions dropdownOptions;
		dropdownOptions.positioning = ZPositioning::Relative;
		dropdownOptions.scaling = ZPositioning::Absolute;
		dropdownOptions.rect = ZRect(1.0f, 1.0f, 200.0f, 400.0f);
		dropdownOptions.color = theme_.primaryColor;
		dropdownOptions.border = ZUIBorder(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f);
		dropdownOptions.zOrderOverride = 1000;
		std::shared_ptr<ZDropMenu> dropMenu = ZDropMenu::Create(menuItemPanel, dropdownOptions, containerScene, theme_);

		itemDropMenus_[menuItemPanel.Handle()] = dropMenu;

		for (const auto& option : suboptions)
		{
			dropMenu->AddMenuItem(option.first, option.second);
		}

		menuItems_.push_back(menuItemPanel);
		menuItemLabels_.push_back(label);
	}
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
