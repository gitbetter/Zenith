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

#include "ZScene.hpp"

const std::string EDITOR_CONFIG_PATH = "/conf.zof";

struct ZEditorConfig
{
    glm::vec4 sizeLimits{ 0.f };
    ZUITheme theme;
};

class ZEditorScene : public ZScene
{
    
public:
    
    ZEditorScene()
        : ZScene("EditorScene"), editorOpen_(true)
    { }
    
    void Initialize() override;
    void Update(double deltaTime) override;
    void CleanUp() override;
    
	ZGameObjectMap& ObjectTemplates() { return gameObjectTemplates_; }
	ZUIElementMap& UITemplates() { return uiElementTemplates_; }
    ZEditorConfig& Config() { return config_; }
    ZGameObjectMap& SelectedObjects() { return selectedObjects_; }
    ZHGameObject EditorCamera() { return editorCamera_; }
    ZSceneSnapshot& LastSceneSnapshot() { return lastSceneSnapshot_; }
    std::shared_ptr<ZScene> ActiveProjectScene() { return activeProjectScene_; }

    void SetSceneSnapshot(ZSceneSnapshot& snapshot) { lastSceneSnapshot_ = snapshot; }
    void SetActiveProjectScene(const std::shared_ptr<ZScene>& activeScene);

    void AddTool(const std::shared_ptr<class ZEditorTool>& tool, const ZHUIElement& layoutRegion);

    ZHGameObject CreateCamera();
    ZHUIElement CreateVerticalRegion(const ZRect& rect, const ZHUIElement& parent = ZHUIElement());
    ZHUIElement CreateHorizontalRegion(const ZRect& rect, const ZHUIElement& parent = ZHUIElement());

private:

    ZGameObjectMap                              gameObjectTemplates_;
    ZUIElementMap                               uiElementTemplates_;

    ZEditorConfig                               config_;
    bool                                        editorOpen_;
    ZGameObjectMap                              selectedObjects_;

    ZHGameObject                                editorCamera_;
    ZHUIElement                                 topPanel_;
    ZHUIElement                                 leftPanel_;
    ZHUIElement                                 centerPanel_;
    ZHUIElement                                 rightPanel_;
    ZHUIElement                                 bottomPanel_;
    std::vector<std::shared_ptr<class ZEditorEntity>> entities_;
    ZSceneSnapshot                              lastSceneSnapshot_;

    std::shared_ptr<ZScene>                     activeProjectScene_;

    void SetupLayoutPanels();
    void SetupInitialTools();
    void Configure(const std::shared_ptr<ZOFNode>& objectTree);
    void Configure(const ZEditorConfig& config);
    void LoadObjectTemplates();

    void HandleResourceLoaded(const std::shared_ptr<class ZResourceLoadedEvent>& event);
    
};
