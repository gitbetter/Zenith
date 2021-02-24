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
#include "ZUIPanel.hpp"
#include "ZUIImage.hpp"
#include "ZDomain.hpp"
#include "ZScene.hpp"
#include "ZTransformGizmo.hpp"

void ZSceneTool::Initialize(const std::shared_ptr<ZScene>& scene) {
    ZEditorTool::Initialize(scene);
    container_->SetColor(glm::vec4(1.f));
    SetupGizmos(scene);
}

void ZSceneTool::OnProjectSceneChanged()
{
    if (activeProjectScene_) {
        container_->SetTexture(activeProjectScene_->TargetTexture());
        for (auto gizmo : gizmos_) {
            gizmo->SetActiveProjectScene(activeProjectScene_);
        }
    }
}

void ZSceneTool::Update()
{

}


void ZSceneTool::AddGizmo(const std::shared_ptr<ZEditorGizmo>& gizmo, const std::shared_ptr<ZScene>& scene)
{
    gizmos_.push_back(gizmo);
    gizmo->Initialize(scene);
}

void ZSceneTool::SetupGizmos(const std::shared_ptr<ZScene>& scene)
{
    AddGizmo(std::make_shared<ZTransformGizmo>(), scene);
}