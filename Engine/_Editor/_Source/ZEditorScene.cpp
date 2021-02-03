/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditor.cpp
 
    Created by Adrian Sanchez on 18/01/21.
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
#include "ZEditorScene.hpp"
#include "ZGame.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZInput.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZSceneReadyEvent.hpp"
#include "ZResourceExtraData.hpp"
#include "ZCamera.hpp"
#include "ZFont.hpp"

#include "ZUIVerticalLayout.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZUIPanel.hpp"

#include "ZMenuBar.hpp"
#include "ZActionBar.hpp"
#include "ZSceneTool.hpp"
#include "ZProjectTool.hpp"
#include "ZConsoleTool.hpp"
#include "ZInspectorTool.hpp"
#include "ZHierarchyTool.hpp"

void ZEditorScene::Initialize() {
	ZServices::EventAgent()->Subscribe(this, &ZEditorScene::HandleResourceLoaded);

	ZServices::LoadZOF(EDITOR_CONFIG_PATH);
	ZServices::LoadZOF(EDITOR_OBJECT_TEMPLATES_PATH);

	PhysicsUniverse()->Pause();

	ZScene::Initialize();

	Play();
}

void ZEditorScene::CleanUp() {
	ZServices::EventAgent()->Unsubscribe(this, &ZEditorScene::HandleResourceLoaded);
	ZScene::CleanUp();
}

std::shared_ptr<ZCamera> ZEditorScene::CreateCamera() {

    std::shared_ptr<ZCamera> camera = ZCamera::Create(glm::vec3(0.f, 15.f, 50.f), glm::vec3(0.f), shared_from_this());
	camera->SetType(ZCameraType::Perspective);
    AddGameObject(camera);
	return camera;
}

std::shared_ptr<ZUIPanel> ZEditorScene::CreateVerticalRegion(const ZRect& rect, std::shared_ptr<ZUIPanel> parent) {
	ZUIElementOptions elementOptions;
	elementOptions.positioning = ZPositioning::Relative;
	elementOptions.rect = rect;
	elementOptions.layout = std::make_shared<ZUIVerticalLayout>();
	std::shared_ptr<ZUIPanel> panel = ZUIPanel::Create(elementOptions, shared_from_this());
	if (parent) {
		parent->AddChild(panel);
	}
	return panel;
}

std::shared_ptr<ZUIPanel> ZEditorScene::CreateHorizontalRegion(const ZRect& rect, std::shared_ptr<ZUIPanel> parent) {
	ZUIElementOptions elementOptions;
	elementOptions.positioning = ZPositioning::Relative;
	elementOptions.rect = rect;
	elementOptions.layout = std::make_shared<ZUIHorizontalLayout>();
	std::shared_ptr<ZUIPanel> panel = ZUIPanel::Create(elementOptions, shared_from_this());
	if (parent) {
		parent->AddChild(panel);
	}
	return panel;
}

void ZEditorScene::AddTool(const std::shared_ptr<ZEditorTool>& tool, const std::shared_ptr<ZUIPanel>& layoutRegion)
{
	tools_.push_back(tool);
	tool->Initialize(shared_from_this());
	layoutRegion->AddChild(tool->Container());
}

void ZEditorScene::SetupLayoutPanels() {

	/*****************************/

	std::shared_ptr<ZUIPanel> windowPanel = CreateVerticalRegion(ZRect(0.f, 0.f, 1.f, 1.f));

	/*************(1)***************/

	ZUIElementOptions elementOptions;

	elementOptions.positioning = ZPositioning::Relative;
	elementOptions.rect = ZRect(0.f, 0.f, 1.0f, 0.07f);
	topPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

	windowPanel->AddChild(topPanel_);

	/*****************************/

	std::shared_ptr<ZUIPanel> contentPanel = CreateHorizontalRegion(ZRect(0.f, 0.f, 1.0f, .93f), windowPanel);

	/************(2)**************/

	elementOptions = ZUIElementOptions();

	elementOptions.positioning = ZPositioning::Relative;
	elementOptions.rect = ZRect(0.f, 0.f, 0.2f, 1.f);
	leftPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

	contentPanel->AddChild(leftPanel_);

	/*****************************/

	std::shared_ptr<ZUIPanel> innerContentPanel = CreateVerticalRegion(ZRect(0.f, 0.f, 0.8f, 1.f), contentPanel);

	/*****************************/

	std::shared_ptr<ZUIPanel> sceneContentPanel = CreateHorizontalRegion(ZRect(0.f, 0.f, 1.0f, 0.75f), innerContentPanel);

	/************(3)**************/

	elementOptions = ZUIElementOptions();

	elementOptions.positioning = ZPositioning::Relative;
	elementOptions.rect = ZRect(0.f, 0.f, 0.7f, 1.0f);
	centerPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

	sceneContentPanel->AddChild(centerPanel_);

	/************(4)**************/

	elementOptions = ZUIElementOptions();

	elementOptions.positioning = ZPositioning::Relative;
	elementOptions.rect = ZRect(0.f, 0.f, 0.3f, 1.0f);
	rightPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

	sceneContentPanel->AddChild(rightPanel_);

	/************(5)**************/

	elementOptions = ZUIElementOptions();

	elementOptions.positioning = ZPositioning::Relative;
	elementOptions.rect = ZRect(0.f, 0.f, 1.0f, .25f);
	bottomPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

	innerContentPanel->AddChild(bottomPanel_);

	/***************************/

	AddUIElement(windowPanel);
}

void ZEditorScene::SetupInitialTools() {
	if (!editorCamera_) {
        editorCamera_ = CreateCamera();
	}

	AddTool(std::make_shared<ZActionBar>(config_.theme), topPanel_);
	AddTool(std::make_shared<ZMenuBar>(config_.theme), topPanel_);
	AddTool(std::make_shared<ZSceneTool>(config_.theme), centerPanel_);
	AddTool(std::make_shared<ZProjectTool>(config_.theme), bottomPanel_);
	AddTool(std::make_shared<ZConsoleTool>(config_.theme), bottomPanel_);
	AddTool(std::make_shared<ZInspectorTool>(config_.theme), leftPanel_);
	AddTool(std::make_shared<ZHierarchyTool>(config_.theme), rightPanel_);
}

void ZEditorScene::Configure(std::shared_ptr<ZOFTree> objectTree) {
	ZEditorConfig config;
	if (objectTree->children.find("CONFIG") != objectTree->children.end()) {
		std::shared_ptr<ZOFObjectNode> configDataNode = std::static_pointer_cast<ZOFObjectNode>(objectTree->children["CONFIG"]);
		if (configDataNode->children.find("THEME") != configDataNode->children.end()) {
			std::shared_ptr<ZOFObjectNode> themeDataNode = std::static_pointer_cast<ZOFObjectNode>(configDataNode->children["THEME"]);
			if (themeDataNode->properties.find("primaryColor") != themeDataNode->properties.end()) {
				std::shared_ptr<ZOFNumberList> primaryColorProp = themeDataNode->properties["primaryColor"]->Value<ZOFNumberList>(0);
				config.theme.primaryColor = glm::vec4(primaryColorProp->value[0], primaryColorProp->value[1], primaryColorProp->value[2], 1.f);
			}
			if (themeDataNode->properties.find("secondaryColor") != themeDataNode->properties.end()) {
				std::shared_ptr<ZOFNumberList> secondaryColorProp = themeDataNode->properties["secondaryColor"]->Value<ZOFNumberList>(0);
				config.theme.secondaryColor = glm::vec4(secondaryColorProp->value[0], secondaryColorProp->value[1], secondaryColorProp->value[2], 1.f);
			}
			if (themeDataNode->properties.find("buttonColor") != themeDataNode->properties.end()) {
				std::shared_ptr<ZOFNumberList> buttonColorProp = themeDataNode->properties["buttonColor"]->Value<ZOFNumberList>(0);
				config.theme.buttonColor = glm::vec4(buttonColorProp->value[0], buttonColorProp->value[1], buttonColorProp->value[2], 1.f);
			}
			if (themeDataNode->properties.find("textColor") != themeDataNode->properties.end()) {
				std::shared_ptr<ZOFNumberList> textColorProp = themeDataNode->properties["textColor"]->Value<ZOFNumberList>(0);
				config.theme.textColor = glm::vec4(textColorProp->value[0], textColorProp->value[1], textColorProp->value[2], 1.f);
			}
			if (themeDataNode->properties.find("highlightColor") != themeDataNode->properties.end()) {
				std::shared_ptr<ZOFNumberList> highlightColorProp = themeDataNode->properties["highlightColor"]->Value<ZOFNumberList>(0);
				config.theme.highlightColor = glm::vec4(highlightColorProp->value[0], highlightColorProp->value[1], highlightColorProp->value[2], 1.f);
			}
			if (themeDataNode->properties.find("selectedColor") != themeDataNode->properties.end()) {
				std::shared_ptr<ZOFNumberList> selectedColorProp = themeDataNode->properties["selectedColor"]->Value<ZOFNumberList>(0);
				config.theme.selectedColor = glm::vec4(selectedColorProp->value[0], selectedColorProp->value[1], selectedColorProp->value[2], 1.f);
			}
			if (themeDataNode->properties.find("font") != themeDataNode->properties.end()) {
				config.theme.font = themeDataNode->properties["font"]->Value<ZOFString>(0)->value;
				config.theme.fontSize = themeDataNode->properties["font"]->Value<ZOFNumber>(1)->value;
			}
		}
	}
	Configure(config);
}

void ZEditorScene::Configure(ZEditorConfig config) {
	config_ = config;

	if (!config_.theme.font.empty()) {
		ZResource fontResource(config_.theme.font, ZResourceType::Font);
		ZServices::ResourceCache()->RequestHandle(fontResource);
	}

	SetupLayoutPanels();
	SetupInitialTools();
}

void ZEditorScene::LoadObjectTemplates(std::shared_ptr<ZOFTree> objectTree) {
	gameObjectTemplates_ = ZGameObject::Load(objectTree, shared_from_this());
	uiElementTemplates_ = ZUIElement::Load(objectTree, shared_from_this());
}

void ZEditorScene::Update(double deltaTime) {
	for (std::shared_ptr<ZEditorTool> tool : tools_) {
		tool->Update();
	}
	ZScene::Update(deltaTime);
}

void ZEditorScene::HandleResourceLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event) {
	ZResource& resource = event->Handle()->Resource();

	if (resource.type == ZResourceType::ZOF && resource.name == EDITOR_CONFIG_PATH) {
		std::shared_ptr<ZZOFResourceExtraData> zofData = std::static_pointer_cast<ZZOFResourceExtraData>(event->Handle()->ExtraData());
		Configure(zofData->ObjectTree());
	} else if (resource.type == ZResourceType::ZOF && resource.name == EDITOR_OBJECT_TEMPLATES_PATH) {
		std::shared_ptr<ZZOFResourceExtraData> zofData = std::static_pointer_cast<ZZOFResourceExtraData>(event->Handle()->ExtraData());
		LoadObjectTemplates(zofData->ObjectTree());
	} else if (resource.type == ZResourceType::Font && resource.name == config_.theme.font) {
		
	}
}