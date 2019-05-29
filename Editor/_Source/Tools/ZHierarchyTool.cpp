/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZHierarchyTool.cpp
 
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

#include "ZHierarchyTool.hpp"
#include "ZGame.hpp"
#include "ZScene.hpp"
#include "ZGameObject.hpp"

void ZHierarchyTool::Begin() {
	ImGui::Begin(name_.c_str());
}

void ZHierarchyTool::Update() {
    ZGameObjectMap& gameObjects = zenith::Game()->ActiveScene()->GameObjects();
    for (ZGameObjectMap::iterator it = gameObjects.begin(), end = gameObjects.end(); it != end; it++) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
        if (ImGui::TreeNodeEx(it->second->ID().c_str(), ImGuiTreeNodeFlags_FramePadding, "%s", it->second->Name().c_str())) {
            ImGui::TreePop();
        }
        ImGui::PopStyleVar();
    }
}

void ZHierarchyTool::End() {
	ImGui::End();
}
