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
    ZGameObjectList& objects = zenith::Game()->ActiveScene()->Root()->Children();
    for (ZGameObjectList::iterator it = objects.begin(), end = objects.end(); it != end; it++)
        DrawGameObjectNode(*it);
}

void ZHierarchyTool::DrawGameObjectNode(std::shared_ptr<ZGameObject> gameObject) {
    if (gameObject == editor_->EditorCamera() || gameObject->RenderPass() == ZRenderPass::Sky) return;

    bool selected = editor_->SelectedObjects().find(gameObject->ID()) != editor_->SelectedObjects().end();
    std::string name = gameObject->Name().empty() ? gameObject->ID() : gameObject->Name();
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    if (ImGui::TreeNodeEx(gameObject->ID().c_str(),
                          ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0) | (gameObject->Children().empty() ? ImGuiTreeNodeFlags_Leaf : 0),
                          "%s", name.c_str())) {
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
    
    if (!ImGui::GetDragDropPayload() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover)) {
        ImGui::SetDragDropPayload("ZGameObject", gameObject.get(), sizeof(ZGameObject));
        ImGui::Text("%s", name.c_str());
        ImGui::EndDragDropSource();
    }
    
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ZGameObject", ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            ZGameObject* otherObjectPtr = static_cast<ZGameObject*>(payload->Data);
            ZGameObjectMap& gameObjects = zenith::Game()->ActiveScene()->GameObjects();
            if (gameObjects.find(otherObjectPtr->ID()) != gameObjects.end()) {
                std::shared_ptr<ZGameObject> other = gameObjects[otherObjectPtr->ID()];
                if (parentObjectPairs_.find(other) == parentObjectPairs_.end())
                    parentObjectPairs_[other] = gameObject;
            }
        }
        ImGui::EndDragDropTarget();
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
    for (auto pair : parentObjectPairs_) {
        pair.second->AddChild(pair.first);
    }
    parentObjectPairs_.clear();
    
	ImGui::End();
}
