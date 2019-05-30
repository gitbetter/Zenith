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
#include "ZEditor.hpp"
#include "ZGame.hpp"
#include "ZInput.hpp"
#include "ZScene.hpp"
#include "ZGameObject.hpp"

void ZHierarchyTool::Begin() {
	ImGui::Begin(name_.c_str());
}

void ZHierarchyTool::Update() {
    ZGameObjectMap& gameObjects = zenith::Game()->ActiveScene()->GameObjects();
    for (ZGameObjectMap::iterator it = gameObjects.begin(), end = gameObjects.end(); it != end; it++) {
		if (it->second == editor_->EditorCamera()) continue;
        DrawGameObjectNode(it->second);
    }
}

void ZHierarchyTool::DrawGameObjectNode(std::shared_ptr<ZGameObject> gameObject) {
    bool selected = editor_->SelectedObjects().find(gameObject->ID()) != editor_->SelectedObjects().end();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    if (ImGui::TreeNodeEx(gameObject->ID().c_str(), ImGuiTreeNodeFlags_FramePadding | (selected ? ImGuiTreeNodeFlags_Selected : 0), "%s", gameObject->ID().c_str())) {
        for (ZGameObjectList::const_iterator it = gameObject->Children().cbegin(), end = gameObject->Children().cend(); it != end; it++)
            DrawGameObjectNode(*it);
        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
    if (ImGui::IsItemClicked()) {
        if (!zenith::Input()->Key(ZKEY_LEFT_CONTROL) && !zenith::Input()->Key(ZKEY_RIGHT_CONTROL)) {
            editor_->SelectedObjects().clear();
        }
        editor_->SelectedObjects()[gameObject->ID()] = gameObject;
    }
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("ZGameObject", gameObject.get(), sizeof(ZGameObject));
        ImGui::Text("%s", gameObject->ID().c_str());
        ImGui::EndDragDropSource();
    }
    ImGui::PushID(gameObject->ID().c_str());
    if(ImGui::BeginPopupContextItem()) {
        // TODO: Polish this up
        static char buf[128] = "";
        ImGui::InputText("Rename", buf, 128);
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void ZHierarchyTool::End() {
	ImGui::End();
}
