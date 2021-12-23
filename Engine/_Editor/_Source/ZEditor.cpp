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
#include "ZAssets.hpp"
#include "ZEditor.hpp"
#include "ZEditorScene.hpp"
#include "ZDevResourceFile.hpp"
#include "ZSceneReadyEvent.hpp"
#include "ZSkybox.hpp"
#include "ZCamera.hpp"
#include "ZLight.hpp"
#include "ZGraphicsComponent.hpp"

void ZEditor::Setup() {
    ZGame::Setup();

	ZServices::ResourceImporter()->RegisterResourceFile(std::shared_ptr<ZDevResourceFile>(new ZDevResourceFile(std::string(EDITOR_ROOT) + "/_Assets")));

    ZAssets::FontManager()->CreateAsync("/Fonts/earth_orbiter/earthorbiter.ttf", 64);

    ZServices::EventAgent()->Subscribe(this, &ZEditor::HandleSceneLoaded);

    CreateDefaultProject();

	editorScene_ = ZScene::LoadIn<ZEditorScene>(shared_from_this());
}

void ZEditor::CreateDefaultProject()
{
	project_ = ZGame::Create("Untitled");

    ZGameOptions options;
    options.domain.windowSize.x = 1920;
    options.domain.windowSize.y = 1080;
    options.domain.maximized = false;
    options.domain.offline = true;
    options.graphics.hasPBR = false;
    options.graphics.drawGrid = true;
    project_->Initialize(options);
    project_->SetTopLevel(false);

    CreateDefaultScene();

    OnUpdateTick([this]()
    {
		if (project_->ActiveScene() != editorScene_->ActiveProjectScene())
		{
            editorScene_->SetActiveProjectScene(project_->ActiveScene());
        }
        project_->Tick();
    });
}

void ZEditor::CreateDefaultScene()
{
    std::shared_ptr<ZScene> scene = ZScene::LoadIn<ZScene>(project_);
    scene->Initialize();

	ZHGameObject camera = ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Camera);
	ZAssets::GameObjectManager()->SetName(camera, "Camera");
	ZAssets::GameObjectManager()->SetPosition(camera, glm::vec3(-5.0f, 15.0f, 50.0f));
	ZAssets::GameObjectManager()->Dereference<ZCamera>(camera)->movementSpeed = 20.0f;
	ZAssets::GameObjectManager()->Dereference<ZCamera>(camera)->cameraType = ZCameraType::Perspective;
	ZAssets::GameObjectManager()->Dereference<ZCamera>(camera)->movementStyle = ZCameraMovementStyle::Follow;

	ZHGameObject light = ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Light);
	ZAssets::GameObjectManager()->SetName(light, "Light");
	ZAssets::GameObjectManager()->Dereference<ZLight>(light)->lightProperties.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	ZAssets::GameObjectManager()->Dereference<ZLight>(light)->lightProperties.color = glm::vec4(0.95f, 0.95f, 0.95f, 1.0f);
	ZAssets::GameObjectManager()->Dereference<ZLight>(light)->lightType = ZLightType::Directional;

	ZHGameObject plane = ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Custom);
	ZAssets::GameObjectManager()->SetName(plane, "Plane");
	ZAssets::GameObjectManager()->SetPosition(plane, glm::vec3(0.0f, 0.0f, 0.0f));
	ZAssets::GameObjectManager()->SetScale(plane, glm::vec3(50.0f, 0.1f, 50.0f));
	ZHComponent planeGraphicsComp = ZAssets::ComponentManager()->CreateIn(ZComponentType::Graphics, plane);
	ZHModel planeModel = ZAssets::ModelManager()->Create(ZModelType::Plane);
	ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(planeGraphicsComp)->Initialize(planeModel);

    scene->AddGameObject(camera);
    scene->AddGameObject(light);
    scene->AddGameObject(plane);
}

void ZEditor::HandleSceneLoaded(const std::shared_ptr<ZSceneReadyEvent>& event)
{
    LOG("Scene '" + event->Scene()->Name() + "' loaded", ZSeverity::Info);
    auto scene = event->Scene();

    if (scene)
    {
        scene->Pause();
    }
}

void ZEditor::CleanUp()
{
    if (project_)
    {
        project_->CleanUp();
    }
	ZGame::CleanUp();
}