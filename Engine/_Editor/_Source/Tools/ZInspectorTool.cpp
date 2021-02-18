/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZInspectorTool.cpp
 
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

#include "ZInspectorTool.hpp"
#include "ZVec3Field.hpp"
#include "ZUIPanel.hpp"

void ZInspectorTool::Initialize(const std::shared_ptr<ZScene>& scene) {
    ZEditorTool::Initialize(scene);
    container_->SetColor(theme_.secondaryColor);

    ZUIElementOptions fieldOptions;
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.05f, 0.05f, 0.9f, 0.05f);
    fieldOptions.maxSize = glm::vec2(0.f, 20.f);
    fieldOptions.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.f);

    AddControl(ZVec3Field::Create("Position", fieldOptions, scene, theme_));
}

void ZInspectorTool::Update()
{
    for (auto control : controls_) {
        control->Update();
    }
}

void ZInspectorTool::CleanUp()
{
    for (auto control : controls_) {
        control->CleanUp();
    }
}