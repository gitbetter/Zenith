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

#include "ZEditorScene.hpp"
#include "ZServices.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZGame.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZInput.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZCamera.hpp"
#include "ZFont.hpp"
#include "ZDomain.hpp"

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
#include "ZFrameStatsDisplay.hpp"

void ZEditorScene::Initialize()
{
    ZServices::EventAgent()->Subscribe(this, &ZEditorScene::HandleResourceLoaded);

    ZServices::LoadZOF(EDITOR_CONFIG_PATH);

    LoadObjectTemplates();

    PhysicsUniverse()->Pause();

    ZScene::Initialize();

    Play();
}

void ZEditorScene::Update(double deltaTime)
{
    ZScene::Update(deltaTime);

    for (std::shared_ptr<ZEditorEntity> entity : entities_)
    {
        entity->Update();
    }
}

void ZEditorScene::CleanUp()
{
    ZServices::EventAgent()->Unsubscribe(this, &ZEditorScene::HandleResourceLoaded);

    for (std::shared_ptr<ZEditorEntity> entity : entities_)
    {
        entity->CleanUp();
    }

    ZScene::CleanUp();
}

ZHGameObject ZEditorScene::CreateCamera()
{
    ZHGameObject camera = ZServices::GameObjectManager()->Create(ZGameObjectType::Camera);
    ZServices::GameObjectManager()->SetScene(camera, shared_from_this());
    ZServices::GameObjectManager()->SetPosition(camera, glm::vec3(0.f, 15.f, 50.f));
    ZServices::GameObjectManager()->Dereference<ZCamera>(camera)->cameraType = ZCameraType::Perspective;
    AddGameObject(camera);
    return camera;
}

ZHUIElement ZEditorScene::CreateVerticalRegion(const ZRect& rect, const ZHUIElement& parent)
{
    ZUIElementOptions elementOptions;
    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = rect;
    elementOptions.layout = std::make_shared<ZUIVerticalLayout>();
    ZHUIElement panel = ZServices::UIElementManager()->Create(ZUIElementType::Panel, elementOptions, ZHUIElement(), shared_from_this());

    if (!parent.IsNull())
    {
        ZServices::UIElementManager()->AddChild(parent, panel);
    }

    return panel;
}

ZHUIElement ZEditorScene::CreateHorizontalRegion(const ZRect& rect, const ZHUIElement& parent)
{
    ZUIElementOptions elementOptions;
    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = rect;
    elementOptions.layout = std::make_shared<ZUIHorizontalLayout>();
    ZHUIElement panel = ZServices::UIElementManager()->Create(ZUIElementType::Panel, elementOptions, ZHUIElement(), shared_from_this());

	if (!parent.IsNull())
	{
		ZServices::UIElementManager()->AddChild(parent, panel);
	}

	return panel;
}

void ZEditorScene::SetActiveProjectScene(const std::shared_ptr<ZScene>& activeScene)
{
    activeProjectScene_ = activeScene;
    for (std::shared_ptr<ZEditorEntity> entity : entities_)
    {
        entity->SetActiveProjectScene(activeProjectScene_);
    }
}

void ZEditorScene::AddTool(const std::shared_ptr<ZEditorTool>& tool, const ZHUIElement& layoutRegion)
{
    entities_.push_back(tool);
    tool->Initialize(shared_from_this());
    tool->SetActiveProjectScene(activeProjectScene_);
    ZServices::UIElementManager()->AddChild(layoutRegion, tool->Container());
}

void ZEditorScene::SetupLayoutPanels()
{

    /*****************************/

    ZHUIElement windowPanel = CreateVerticalRegion(ZRect(0.f, 0.f, 1.f, 1.f));

    /*************(1)***************/

    ZUIElementOptions elementOptions;

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 1.0f, 0.07f);
    topPanel_ = ZServices::UIElementManager()->Create(ZUIElementType::Panel, elementOptions, ZHUIElement(), shared_from_this());

    ZServices::UIElementManager()->AddChild(windowPanel, topPanel_);

    /*****************************/

    ZHUIElement contentPanel = CreateHorizontalRegion(ZRect(0.f, 0.f, 1.0f, .93f), windowPanel);

    /************(2)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 0.2f, 1.f);
    elementOptions.minSize = glm::vec2(config_.sizeLimits.x / 2.5f, 0.f);
    leftPanel_ = ZServices::UIElementManager()->Create(ZUIElementType::Panel, elementOptions, ZHUIElement(), shared_from_this());

    ZServices::UIElementManager()->AddChild(contentPanel, leftPanel_);

    /*****************************/

    ZHUIElement innerContentPanel = CreateVerticalRegion(ZRect(0.f, 0.f, 0.8f, 1.f), contentPanel);

    /*****************************/

    ZHUIElement sceneContentPanel = CreateHorizontalRegion(ZRect(0.f, 0.f, 1.0f, 0.75f), innerContentPanel);

    /************(3)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 0.7f, 1.0f);
    elementOptions.minSize = glm::vec2(config_.sizeLimits.x / 5.f, 0.f);
    centerPanel_ = ZServices::UIElementManager()->Create(ZUIElementType::Panel, elementOptions, ZHUIElement(), shared_from_this());

    ZServices::UIElementManager()->AddChild(sceneContentPanel, centerPanel_);

    /************(4)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 0.3f, 1.0f);
    elementOptions.minSize = glm::vec2(config_.sizeLimits.x / 2.5f, 0.f);
    rightPanel_ = ZServices::UIElementManager()->Create(ZUIElementType::Panel, elementOptions, ZHUIElement(), shared_from_this());

    ZServices::UIElementManager()->AddChild(sceneContentPanel, rightPanel_);

    /************(5)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 1.0f, .25f);
    bottomPanel_ = ZServices::UIElementManager()->Create(ZUIElementType::Panel, elementOptions, ZHUIElement(), shared_from_this());

    ZServices::UIElementManager()->AddChild(innerContentPanel, bottomPanel_);

    /***************************/

    AddUIElement(windowPanel);
}

void ZEditorScene::SetupInitialTools()
{
    if (!editorCamera_)
    {
        editorCamera_ = CreateCamera();
    }

    AddTool(std::make_shared<ZActionBar>(config_.theme), topPanel_);
    AddTool(std::make_shared<ZMenuBar>(config_.theme), topPanel_);
    AddTool(std::make_shared<ZSceneTool>(config_.theme), centerPanel_);
    AddTool(std::make_shared<ZFrameStatsDisplay>(config_.theme), centerPanel_);
    AddTool(std::make_shared<ZProjectTool>(config_.theme), bottomPanel_);
    AddTool(std::make_shared<ZConsoleTool>(config_.theme), bottomPanel_);
    AddTool(std::make_shared<ZInspectorTool>(config_.theme), leftPanel_);
    AddTool(std::make_shared<ZHierarchyTool>(config_.theme), rightPanel_);
}

void ZEditorScene::Configure(const std::shared_ptr<ZOFNode>& objectTree)
{
    ZEditorConfig config;
    for (auto it = objectTree->children.begin(); it != objectTree->children.end(); it++)
    {
        std::shared_ptr<ZOFObjectNode> dataNode = std::static_pointer_cast<ZOFObjectNode>(it->second);
        if (dataNode == nullptr || dataNode->type != ZOFObjectType::Config)
        {
            continue;
        }
        
		if (dataNode->properties.find("editorSizeLimits") != dataNode->properties.end())
		{
			std::shared_ptr<ZOFNumberList> sizeLimitsProp = dataNode->properties["editorSizeLimits"]->Value<ZOFNumberList>(0);
			config.sizeLimits = glm::vec4(sizeLimitsProp->value[0], sizeLimitsProp->value[1], sizeLimitsProp->value[2], sizeLimitsProp->value[3]);
		}

        for (auto innerIt = dataNode->children.begin(); innerIt != dataNode->children.end(); innerIt++)
        {
			std::shared_ptr<ZOFObjectNode> themeNode = std::static_pointer_cast<ZOFObjectNode>(innerIt->second);
			if (themeNode == nullptr || themeNode->type != ZOFObjectType::Theme)
			{
				continue;
			}

			if (themeNode->properties.find("primaryColor") != themeNode->properties.end())
            {
                std::shared_ptr<ZOFNumberList> primaryColorProp = themeNode->properties["primaryColor"]->Value<ZOFNumberList>(0);
                config.theme.primaryColor = glm::vec4(primaryColorProp->value[0], primaryColorProp->value[1], primaryColorProp->value[2], 1.f);
            }
            if (themeNode->properties.find("secondaryColor") != themeNode->properties.end())
            {
                std::shared_ptr<ZOFNumberList> secondaryColorProp = themeNode->properties["secondaryColor"]->Value<ZOFNumberList>(0);
                config.theme.secondaryColor = glm::vec4(secondaryColorProp->value[0], secondaryColorProp->value[1], secondaryColorProp->value[2], 1.f);
            }
            if (themeNode->properties.find("buttonColor") != themeNode->properties.end())
            {
                std::shared_ptr<ZOFNumberList> buttonColorProp = themeNode->properties["buttonColor"]->Value<ZOFNumberList>(0);
                config.theme.buttonColor = glm::vec4(buttonColorProp->value[0], buttonColorProp->value[1], buttonColorProp->value[2], 1.f);
            }
            if (themeNode->properties.find("textColor") != themeNode->properties.end())
            {
                std::shared_ptr<ZOFNumberList> textColorProp = themeNode->properties["textColor"]->Value<ZOFNumberList>(0);
                config.theme.textColor = glm::vec4(textColorProp->value[0], textColorProp->value[1], textColorProp->value[2], 1.f);
            }
            if (themeNode->properties.find("highlightColor") != themeNode->properties.end())
            {
                std::shared_ptr<ZOFNumberList> highlightColorProp = themeNode->properties["highlightColor"]->Value<ZOFNumberList>(0);
                config.theme.highlightColor = glm::vec4(highlightColorProp->value[0], highlightColorProp->value[1], highlightColorProp->value[2], 1.f);
            }
            if (themeNode->properties.find("selectedColor") != themeNode->properties.end())
            {
                std::shared_ptr<ZOFNumberList> selectedColorProp = themeNode->properties["selectedColor"]->Value<ZOFNumberList>(0);
                config.theme.selectedColor = glm::vec4(selectedColorProp->value[0], selectedColorProp->value[1], selectedColorProp->value[2], 1.f);
            }
            if (themeNode->properties.find("font") != themeNode->properties.end())
            {
                config.theme.font = themeNode->properties["font"]->Value<ZOFString>(0)->value;
                config.theme.fontSize = themeNode->properties["font"]->Value<ZOFNumber>(1)->value;
			}
        }
    }

    Configure(config);
}

void ZEditorScene::Configure(const ZEditorConfig& config)
{
    config_ = config;

    if (!config_.theme.font.empty())
    {
        ZResourceData::ptr fontResource = std::make_shared<ZResourceData>(config_.theme.font, ZResourceType::Font);
        ZServices::ResourceImporter()->GetDataAsync(fontResource);
    }

    Domain()->SetSizeLimits(
        glm::vec2(config.sizeLimits.x, config.sizeLimits.y),
        glm::vec2(config.sizeLimits.z, config.sizeLimits.w)
    );

    SetupLayoutPanels();
    SetupInitialTools();
}

void ZEditorScene::LoadObjectTemplates()
{
    /******* Empty Object *******/
    ZHGameObject empty = ZServices::GameObjectManager()->Create(ZGameObjectType::Custom);
    ZServices::GameObjectManager()->SetName(empty, "Empty");
    gameObjectTemplates_["Empty"] = empty;
    /****************************/

    /******* Cube Object *******/
	ZHGameObject cube = ZServices::GameObjectManager()->Create(ZGameObjectType::Custom);
	ZServices::GameObjectManager()->SetName(cube, "Cube");
    ZServices::GameObjectManager()->SetPosition(cube, glm::vec3(0.0f, 1.0f, 0.0f));
    ZHComponent cubeGraphicsComp = ZServices::ComponentManager()->CreateIn(ZComponentType::Graphics, cube);
    ZHModel cubeModel = ZServices::ModelManager()->Create(ZModelType::Cube);
    ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(cubeGraphicsComp)->AddModel(cubeModel);
	gameObjectTemplates_["Cube"] = cube;
    /****************************/

    /******* Sphere Object *******/
	ZHGameObject sphere = ZServices::GameObjectManager()->Create(ZGameObjectType::Custom);
	ZServices::GameObjectManager()->SetName(sphere, "Sphere");
	ZServices::GameObjectManager()->SetPosition(sphere, glm::vec3(0.0f, 1.0f, 0.0f));
	ZHComponent sphereGraphicsComp = ZServices::ComponentManager()->CreateIn(ZComponentType::Graphics, sphere);
	ZHModel sphereModel = ZServices::ModelManager()->Create(ZModelType::Sphere);
	ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(sphereGraphicsComp)->AddModel(sphereModel);
	gameObjectTemplates_["Sphere"] = sphere;
	/****************************/

	/******* Plane Object *******/
	ZHGameObject plane = ZServices::GameObjectManager()->Create(ZGameObjectType::Custom);
	ZServices::GameObjectManager()->SetName(plane, "Plane");
	ZServices::GameObjectManager()->SetPosition(plane, glm::vec3(0.0f, 0.0f, 0.0f));
    ZServices::GameObjectManager()->SetScale(plane, glm::vec3(50.0f, 0.1f, 50.0f));
	ZHComponent planeGraphicsComp = ZServices::ComponentManager()->CreateIn(ZComponentType::Graphics, plane);
	ZHModel planeModel = ZServices::ModelManager()->Create(ZModelType::Plane);
	ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(planeGraphicsComp)->AddModel(planeModel);
	gameObjectTemplates_["Plane"] = plane;
	/****************************/

	/******* Camera Object *******/
	ZHGameObject camera = ZServices::GameObjectManager()->Create(ZGameObjectType::Camera);
	ZServices::GameObjectManager()->SetName(camera, "Camera");
	ZServices::GameObjectManager()->SetPosition(camera, glm::vec3(-5.0f, 15.0f, 50.0f));
    ZServices::GameObjectManager()->Dereference<ZCamera>(camera)->movementSpeed = 20.0f;
    ZServices::GameObjectManager()->Dereference<ZCamera>(camera)->cameraType = ZCameraType::Perspective;
    ZServices::GameObjectManager()->Dereference<ZCamera>(camera)->movementStyle = ZCameraMovementStyle::Follow;
	gameObjectTemplates_["Camera"] = camera;
	/****************************/

	/******* Light Object *******/
	ZHGameObject light = ZServices::GameObjectManager()->Create(ZGameObjectType::Light);
	ZServices::GameObjectManager()->SetName(light, "Light");
	ZServices::GameObjectManager()->Dereference<ZLight>(camera)->lightProperties.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	ZServices::GameObjectManager()->Dereference<ZLight>(camera)->lightProperties.color = glm::vec4(0.95f, 0.95f, 0.95f, 1.0f);
	ZServices::GameObjectManager()->Dereference<ZLight>(camera)->lightType = ZLightType::Directional;
	gameObjectTemplates_["Light"] = light;
	/****************************/
}

void ZEditorScene::HandleResourceLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event)
{
    ZResourceData::ptr resource = event->Resource();

    if (resource->type == ZResourceType::ZOF && resource->path == EDITOR_CONFIG_PATH)
    {
        ZZofResourceData::ptr zofData = std::static_pointer_cast<ZZofResourceData>(resource);
        Configure(zofData->objectTree);
    }
    else if (resource->type == ZResourceType::Font && resource->path == config_.theme.font)
    {

    }
}