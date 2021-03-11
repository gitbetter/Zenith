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
#include "ZUIPanel.hpp"
#include "ZUIImage.hpp"
#include "ZDomain.hpp"
#include "ZScene.hpp"
#include "ZTransformGizmo.hpp"
#include "ZUIClicker.hpp"
#include "ZCamera.hpp"
#include "ZFrameStatsDisplay.hpp"

void ZSceneTool::Initialize(const std::shared_ptr<ZScene>& scene) {
    ZEditorTool::Initialize(scene);
    container_->SetColor(glm::vec4(1.f));

    selectClicker_ = std::make_shared<ZUIClicker>();
    selectClicker_->WrapToBounds();
    travelClicker_ = std::make_shared<ZUIClicker>(ZMouse::RIGHT_MB);

    SetupGizmos(scene);
}

void ZSceneTool::SetSelectedObject(const std::shared_ptr<ZGameObject>& object)
{
    selectedObject_ = object;
    if (currentGizmo_) {
        currentGizmo_->SetPosition(selectedObject_->ModelMatrix()[3]);
    }
}

void ZSceneTool::OnProjectSceneChanged()
{
    if (activeProjectScene_) {
        container_->SetTexture(activeProjectScene_->TargetTexture());
        if (currentGizmo_) {
            currentGizmo_->SetActiveProjectScene(activeProjectScene_);
        }
    }
}

void ZSceneTool::Update()
{
    auto rect = container_->CalculatedRect();
    if (currentGizmo_->Showing()) {
        currentGizmo_->Update();
    }

    if (selectClicker_->Release(rect)) {
        if (currentGizmo_->Showing())
            currentGizmo_->Deactivate();
    }
    else if (selectClicker_->Press(rect)) {
        if (currentGizmo_->Showing()) {
            auto transform = selectedObject_->ModelMatrix();
            currentGizmo_->Manipulate(rect, transform);
            selectedObject_->SetModelMatrix(transform);
        }
    }
    else if (selectClicker_->Click(rect)) {
        if (!currentGizmo_->Showing() || !currentGizmo_->TryActivate(rect)) {
            auto cursorPos = ZServices::Input()->GetCursorPosition() - rect.position;
            auto ray = activeProjectScene_->ScreenPointToWorldRay(cursorPos, rect.size);

            ZIntersectHitResult hitResult;
            bool hasHit = activeProjectScene_->RayCast(ray, hitResult);
            std::shared_ptr<ZGameObject> foundObj = hasHit ? activeProjectScene_->FindGameObject(hitResult.objectId) : nullptr;

            if (hasHit && foundObj) {
                SetSelectedObject(foundObj);
                currentGizmo_->Show();
            }
            else if (currentGizmo_->Showing()) {
                currentGizmo_->Hide();
            }
        }
    }

    if (travelClicker_->Release(rect)) {
        activeProjectScene_->ActiveCamera()->DisableLook();
        activeProjectScene_->ActiveCamera()->DisableMovement();
        ZServices::Input()->ReleaseCursor();
    } else if (travelClicker_->Click(rect)) {
        activeProjectScene_->ActiveCamera()->EnableLook();
        activeProjectScene_->ActiveCamera()->EnableMovement();
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