/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditor.hpp
 
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

#pragma once

// Includes
#include "ZScene.hpp"
#include "ZUI.hpp"

// Forward Declarations
class ZEditorTool;
class ZMenuBar;
class ZEvent;
class ZResourceHandle;
class ZUIPanel;

// Definitions
const std::string EDITOR_CONFIG_PATH = "/conf.zof";
const std::string EDITOR_OBJECT_TEMPLATES_PATH = "/object_templates.zof";

struct ZEditorConfig {
    std::string fontPath;
    float fontSize;
    ZUITheme theme;
};

class ZEditorScene : public ZScene {

private:

	ZGameObjectMap                              gameObjectTemplates_;
	ZUIElementMap                               uiElementTemplates_;

	ZEditorConfig                               config_;
    bool                                        editorOpen_;
    ZGameObjectMap                              selectedObjects_;

    std::shared_ptr<ZGameObject>                editorCamera_;
    std::shared_ptr<ZUIPanel>                   topPanel_;
    std::shared_ptr<ZUIPanel>                   leftPanel_;
    std::shared_ptr<ZUIPanel>                   centerPanel_;
    std::shared_ptr<ZUIPanel>                   rightPanel_;
    std::shared_ptr<ZUIPanel>                   bottomPanel_;
	std::vector<std::shared_ptr<ZEditorTool>>   tools_;
    ZSceneSnapshot                              lastSceneSnapshot_;

    
    void SetupLayoutPanels();
    void SetupInitialTools();
	void Configure(std::shared_ptr<ZOFTree> objectTree);
	void Configure(ZEditorConfig config);
	void LoadObjectTemplates(std::shared_ptr<ZOFTree> objectTree);

	void HandleResourceLoaded(const std::shared_ptr<ZEvent>& event);
    void HandleSceneLoaded(const std::shared_ptr<ZEvent>& event);
    
public:
    
    ZEditorScene() : ZScene(), editorOpen_(true) { }
    
    void Initialize() override;
    void Update(double deltaTime) override;
    void CleanUp() override;
    
    std::shared_ptr<ZGameObject> CreateCamera();
    std::shared_ptr<ZUIPanel> CreateVerticalRegion(const ZRect& rect, std::shared_ptr<ZUIPanel> parent = nullptr);
    std::shared_ptr<ZUIPanel> CreateHorizontalRegion(const ZRect& rect, std::shared_ptr<ZUIPanel> parent = nullptr);
    void AddTool(const std::shared_ptr<ZEditorTool>& tool, const std::shared_ptr<ZUIPanel>& layoutRegion);
    
	ZGameObjectMap& ObjectTemplates() { return gameObjectTemplates_; }
	ZUIElementMap& UITemplates() { return uiElementTemplates_; }
    ZEditorConfig& Config() { return config_; }
    ZGameObjectMap& SelectedObjects() { return selectedObjects_; }
	std::shared_ptr<ZGameObject> EditorCamera() { return editorCamera_; }
    ZSceneSnapshot& LastSceneSnapshot() { return lastSceneSnapshot_; }

    void SetSceneSnapshot(ZSceneSnapshot& snapshot) { lastSceneSnapshot_ = snapshot; }
    
};
