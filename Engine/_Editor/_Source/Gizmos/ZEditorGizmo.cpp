/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditorGizmo.cpp

    Created by Adrian Sanchez on 02/20/21.
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

#include "ZEditorGizmo.hpp"
#include "ZGameObject.hpp"

void ZEditorGizmo::Initialize(const std::shared_ptr<ZScene>& scene)
{
    gizmo_ = std::make_shared<ZGameObject>();
    gizmo_->SetRenderOrder(ZRenderLayer::UI);
    gizmo_->SetActive(false);
}

bool ZEditorGizmo::Showing()
{
    return gizmo_->Active();
}

void ZEditorGizmo::SetPosition(const glm::vec3& position)
{
    gizmo_->SetPosition(position);
}

void ZEditorGizmo::Hide()
{
    gizmo_->SetActive(false);
}

void ZEditorGizmo::Show()
{
    gizmo_->SetActive(true);
}
