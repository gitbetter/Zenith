/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZSceneTool.cpp
 
    Created by Adrian Sanchez on 18/05/19.
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

#include "ZSceneTool.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZUIPanel.hpp"
#include "ZUIImage.hpp"
#include "ZDomain.hpp"
#include "ZScene.hpp"
#include "ZTransformGizmo.hpp"
#include "ZCamera.hpp"
#include "ZEditorObjectSelectedEvent.hpp"

void ZSceneTool::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorTool::Initialize(scene);

    ZAssets::UIElementManager()->SetColor(container_, glm::vec4(1.0f));

    selectClicker_.WrapToBounds();
    travelClicker_ = ZUIClicker(ZMouse::RIGHT_MB);

    SetupGizmos(scene);
}

void ZSceneTool::SetSelectedObject(const ZHGameObject& object)
{
    selectedObject_ = object;

    if (currentGizmo_->Showing())
    {
        currentGizmo_->SetPosition(ZAssets::GameObjectManager()->ModelMatrix(selectedObject_)[3]);
    }

    auto selectedEvent = std::make_shared<ZEditorObjectSelectedEvent>(!selectedObject_.IsNull() ? ZAssets::GameObjectManager()->Name(selectedObject_) : "");

    ZServices::EventAgent()->Trigger(selectedEvent);
}

void ZSceneTool::OnProjectSceneChanged()
{
    if (activeProjectScene_)
    {
        ZAssets::UIElementManager()->SetTexture(container_, activeProjectScene_->TargetTexture());

        if (currentGizmo_)
        {
            currentGizmo_->SetActiveProjectScene(activeProjectScene_);
        }
    }
}

void ZSceneTool::Update()
{
    auto rect = ZAssets::UIElementManager()->CalculatedRect(container_);

    if (currentGizmo_->Showing())
    {
        currentGizmo_->Update();
        currentGizmo_->SetPosition(ZAssets::GameObjectManager()->ModelMatrix(selectedObject_)[3]);
    }

    if (selectClicker_.Release(rect))
    {
        if (currentGizmo_->Showing())
        {
            currentGizmo_->Deactivate();
        }
    }
    else if (selectClicker_.Press(rect))
    {
        if (currentGizmo_->Showing())
        {
            auto transform = ZAssets::GameObjectManager()->ModelMatrix(selectedObject_);
            currentGizmo_->Manipulate(rect, transform);
            ZAssets::GameObjectManager()->SetModelMatrix(selectedObject_, transform);
        }
    }
    else if (selectClicker_.Click(rect))
    {
        if (!currentGizmo_->Showing() || !currentGizmo_->TryActivate(rect))
        {
            auto cursorPos = ZServices::Input()->GetCursorPosition() - rect.position;
            auto ray = activeProjectScene_->ScreenPointToWorldRay(cursorPos, rect.size);

            ZIntersectHitResult hitResult;
            bool hasHit = activeProjectScene_->RayCast(ray, hitResult);
            ZHGameObject foundObj = hitResult.objectHandle;

            if (!foundObj.IsNull())
            {
                currentGizmo_->Show();
                SetSelectedObject(foundObj);
            }
            else if (currentGizmo_->Showing())
            {
                currentGizmo_->Hide();
                SetSelectedObject(ZHGameObject());
            }
        }
    }

    if (travelClicker_.Release(rect))
    {
        auto sceneCamera = ZAssets::GameObjectManager()->Dereference<ZCamera>(activeProjectScene_->ActiveCamera());
        sceneCamera->DisableLook();
        sceneCamera->DisableMovement();
        ZServices::Input()->ReleaseCursor();
    }
    else if (travelClicker_.Click(rect))
    {
        auto sceneCamera = ZAssets::GameObjectManager()->Dereference<ZCamera>(activeProjectScene_->ActiveCamera());
        sceneCamera->EnableLook();
        sceneCamera->EnableMovement();
        ZServices::Input()->CaptureCursor();
    }
}


void ZSceneTool::AddGizmo(const std::shared_ptr<ZEditorGizmo>& gizmo, const std::shared_ptr<ZScene>& scene)
{
    gizmos_.push_back(gizmo);
    gizmo->Initialize(scene);
    currentGizmo_ = gizmo;
}

void ZSceneTool::SetupGizmos(const std::shared_ptr<ZScene>& scene)
{
    AddGizmo(std::make_shared<ZTransformGizmo>(), scene);
}