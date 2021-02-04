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
#include "ZEditor.hpp"
#include "ZEditorScene.hpp"
#include "ZAssetStore.hpp"
#include "ZDevResourceFile.hpp"
#include "ZSceneReadyEvent.hpp"

void ZEditor::Setup() {
    ZGame::Setup();

	ZServices::ResourceCache()->RegisterResourceFile(std::shared_ptr<ZDevResourceFile>(new ZDevResourceFile(std::string(EDITOR_ROOT) + "/_Assets")));

	AssetStore()->RegisterFont("/Fonts/earth_orbiter/earthorbiter.ttf");

    ZServices::EventAgent()->Subscribe(this, &ZEditor::HandleSceneLoaded);

    CreateDefaultProject();

	editorScene_ = ZScene::LoadIn<ZEditorScene>(shared_from_this());
}

void ZEditor::CreateDefaultProject()
{
	project_ = ZGame::Create();
    ZGameOptions options;
    options.domain.windowSize.x = 1920;
    options.domain.windowSize.y = 1080;
    options.domain.maximized = false;
    options.domain.offline = true;
    options.graphics.drawCameraDebug = true;
    options.graphics.drawGrid = true;
    options.graphics.drawPhysicsDebug = true;
    project_->Initialize(options);

    ZScene::LoadIn<ZScene>(project_, std::initializer_list<std::string>({ "/demo_scene.zof" }));

    OnUpdateTick([this]() {
        if (project_->ActiveScene() != editorScene_->ActiveProjectScene())
            editorScene_->SetActiveProjectScene(project_->ActiveScene());
        project_->Tick();
    });
}

void ZEditor::HandleSceneLoaded(const std::shared_ptr<ZSceneReadyEvent>& event)
{
    LOG("Scene '" + event->Scene()->Name() + "' loaded", ZSeverity::Info);
    auto scene = event->Scene();

    if (scene)
    {
        scene->Play();
    }
}

void ZEditor::CleanUp() {
    if (project_)
        project_->CleanUp();
	ZGame::CleanUp();
}