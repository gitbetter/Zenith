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

void ZEditorScene::Initialize() {
    ZServices::EventAgent()->Subscribe(this, &ZEditorScene::HandleResourceLoaded);

    ZServices::LoadZOF(EDITOR_CONFIG_PATH);
    ZServices::LoadZOF(EDITOR_OBJECT_TEMPLATES_PATH);

    PhysicsUniverse()->Pause();

    ZScene::Initialize();

    Play();
}

void ZEditorScene::Update(double deltaTime) {
    ZScene::Update(deltaTime);
    for (std::shared_ptr<ZEditorEntity> entity : entities_) {
        entity->Update();
    }
}

void ZEditorScene::CleanUp() {
    ZServices::EventAgent()->Unsubscribe(this, &ZEditorScene::HandleResourceLoaded);
    for (std::shared_ptr<ZEditorEntity> entity : entities_) {
        entity->CleanUp();
    }
    ZScene::CleanUp();
}

std::shared_ptr<ZCamera> ZEditorScene::CreateCamera() {

    std::shared_ptr<ZCamera> camera = ZCamera::Create(glm::vec3(0.f, 15.f, 50.f), glm::vec3(0.f), glm::vec3(1.f), shared_from_this());
    camera->SetType(ZCameraType::Perspective);
    AddGameObject(camera);
    return camera;
}

std::shared_ptr<ZUIPanel> ZEditorScene::CreateVerticalRegion(const ZRect& rect, std::shared_ptr<ZUIPanel> parent) {
    ZUIElementOptions elementOptions;
    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
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
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = rect;
    elementOptions.layout = std::make_shared<ZUIHorizontalLayout>();
    std::shared_ptr<ZUIPanel> panel = ZUIPanel::Create(elementOptions, shared_from_this());
    if (parent) {
        parent->AddChild(panel);
    }
    return panel;
}

void ZEditorScene::SetActiveProjectScene(const std::shared_ptr<ZScene>& activeScene)
{
    activeProjectScene_ = activeScene;
    for (std::shared_ptr<ZEditorEntity> entity : entities_) {
        entity->SetActiveProjectScene(activeProjectScene_);
    }
}

void ZEditorScene::AddTool(const std::shared_ptr<ZEditorTool>& tool, const std::shared_ptr<ZUIPanel>& layoutRegion)
{
    entities_.push_back(tool);
    tool->Initialize(shared_from_this());
    tool->SetActiveProjectScene(activeProjectScene_);
    layoutRegion->AddChild(tool->Container());
}

void ZEditorScene::SetupLayoutPanels() {

    /*****************************/

    std::shared_ptr<ZUIPanel> windowPanel = CreateVerticalRegion(ZRect(0.f, 0.f, 1.f, 1.f));

    /*************(1)***************/

    ZUIElementOptions elementOptions;

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 1.0f, 0.07f);
    topPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

    windowPanel->AddChild(topPanel_);

    /*****************************/

    std::shared_ptr<ZUIPanel> contentPanel = CreateHorizontalRegion(ZRect(0.f, 0.f, 1.0f, .93f), windowPanel);

    /************(2)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 0.2f, 1.f);
    elementOptions.minSize = glm::vec2(config_.sizeLimits.x / 2.5f, 0.f);
    leftPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

    contentPanel->AddChild(leftPanel_);

    /*****************************/

    std::shared_ptr<ZUIPanel> innerContentPanel = CreateVerticalRegion(ZRect(0.f, 0.f, 0.8f, 1.f), contentPanel);

    /*****************************/

    std::shared_ptr<ZUIPanel> sceneContentPanel = CreateHorizontalRegion(ZRect(0.f, 0.f, 1.0f, 0.75f), innerContentPanel);

    /************(3)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 0.7f, 1.0f);
    elementOptions.minSize = glm::vec2(config_.sizeLimits.x / 5.f, 0.f);
    centerPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

    sceneContentPanel->AddChild(centerPanel_);

    /************(4)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
    elementOptions.rect = ZRect(0.f, 0.f, 0.3f, 1.0f);
    elementOptions.minSize = glm::vec2(config_.sizeLimits.x / 2.5f, 0.f);
    rightPanel_ = ZUIPanel::Create(elementOptions, shared_from_this());

    sceneContentPanel->AddChild(rightPanel_);

    /************(5)**************/

    elementOptions = ZUIElementOptions();

    elementOptions.positioning = ZPositioning::Relative;
    elementOptions.scaling = ZPositioning::Relative;
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
    AddTool(std::make_shared<ZFrameStatsDisplay>(config_.theme), centerPanel_);
    AddTool(std::make_shared<ZProjectTool>(config_.theme), bottomPanel_);
    AddTool(std::make_shared<ZConsoleTool>(config_.theme), bottomPanel_);
    AddTool(std::make_shared<ZInspectorTool>(config_.theme), leftPanel_);
    AddTool(std::make_shared<ZHierarchyTool>(config_.theme), rightPanel_);
}

void ZEditorScene::Configure(std::shared_ptr<ZOFNode> objectTree) {
    ZEditorConfig config;
    if (objectTree->children.find("CONFIG") != objectTree->children.end()) {
        std::shared_ptr<ZOFObjectNode> configDataNode = std::static_pointer_cast<ZOFObjectNode>(objectTree->children["CONFIG"]);

        if (configDataNode->properties.find("editorSizeLimits") != configDataNode->properties.end()) {
            std::shared_ptr<ZOFNumberList> sizeLimitsProp = configDataNode->properties["editorSizeLimits"]->Value<ZOFNumberList>(0);
            config.sizeLimits = glm::vec4(sizeLimitsProp->value[0], sizeLimitsProp->value[1], sizeLimitsProp->value[2], sizeLimitsProp->value[3]);
        }

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

void ZEditorScene::LoadObjectTemplates(std::shared_ptr<ZOFNode> objectTree) {
    auto gameObjects = ZGameObject::Load(objectTree, shared_from_this());
    for (auto object : gameObjects)
        gameObjectTemplates_[object->ID()] = object;

    auto uiElements = ZUIElement::Load(objectTree, shared_from_this());
    for (auto el : uiElements)
        uiElementTemplates_[el->ID()] = el;
}

void ZEditorScene::HandleResourceLoaded(const std::shared_ptr<ZResourceLoadedEvent>& event) {
    ZResourceData::ptr resource = event->Resource();

    if (resource->type == ZResourceType::ZOF && resource->path == EDITOR_CONFIG_PATH) {
        ZZofResourceData::ptr zofData = std::static_pointer_cast<ZZofResourceData>(resource);
        Configure(zofData->objectTree);
    }
    else if (resource->type == ZResourceType::ZOF && resource->path == EDITOR_OBJECT_TEMPLATES_PATH) {
        ZZofResourceData::ptr zofData = std::static_pointer_cast<ZZofResourceData>(resource);
        LoadObjectTemplates(zofData->objectTree);
    }
    else if (resource->type == ZResourceType::Font && resource->path == config_.theme.font) {

    }
}