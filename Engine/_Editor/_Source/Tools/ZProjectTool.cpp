/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZProjectTool.cpp
 
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

#include "ZProjectTool.hpp"
#include "ZAssets.hpp"
#include "ZUIPanel.hpp"

void ZProjectTool::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorTool::Initialize(scene);

    ZAssets::UIElementManager()->SetColor(container_, theme_.primaryColor);
    ZAssets::UIElementManager()->SetBorder(container_, ZUIBorder(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 2.0f, 0.0f));
}

void ZProjectTool::Update()
{
}
