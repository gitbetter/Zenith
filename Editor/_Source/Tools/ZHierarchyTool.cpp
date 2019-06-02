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
	ImGui::PushFont(editor_->HeaderFont());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 40));
	ImGui::Text("%s", zenith::Game()->ActiveScene()->Name().c_str());
	ImGui::PopStyleVar();
	ImGui::PopFont();
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 50);
	ImGui::Text("%d objects", zenith::Game()->ActiveScene()->GameObjects().size() - 1); // - 1 because editor camera doesn't count

    ZGameObjectList& objects = zenith::Game()->ActiveScene()->Root()->Children();
    for (ZGameObjectList::iterator it = objects.begin(), end = objects.end(); it != end; it++)
        DrawGameObjectNode(*it);
}

void ZHierarchyTool::DrawGameObjectNode(std::shared_ptr<ZGameObject> gameObject) {
    if (gameObject == editor_->EditorCamera() || gameObject->RenderPass() == ZRenderPass::Sky) return;

    bool selected = editor_->SelectedObjects().find(gameObject->ID()) != editor_->SelectedObjects().end();
    std::string name = gameObject->Name().empty() ? gameObject->ID() : gameObject->Name();
    
	unsigned int flags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen | (selected ? ImGuiTreeNodeFlags_Selected : 0) | (gameObject->Children().empty() ? ImGuiTreeNodeFlags_Leaf : 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
	bool open = ImGui::TreeNodeEx(gameObject->ID().c_str(), flags, "%s", name.c_str());
	ImGui::PopStyleVar();

	DrawContextMenu(gameObject);
	SelectObjectsIfClicked(gameObject);
	HandleDragDrop(gameObject, name);

    if (open) {
        for (ZGameObjectList::const_iterator it = gameObject->Children().cbegin(), end = gameObject->Children().cend(); it != end; it++)
            DrawGameObjectNode(*it);
        ImGui::TreePop();
    }
}

void ZHierarchyTool::DrawContextMenu(std::shared_ptr<ZGameObject> &gameObject) {
	ImGui::PushID(gameObject->ID().c_str());
	static char nameBuffer[512] = "";
	if (ImGui::BeginPopupContextItem()) {
		if (strcmp(nameBuffer, gameObject->Name().c_str()) != 0) {
			memcpy(nameBuffer, gameObject->Name().c_str(), gameObject->Name().size());
		}

		if (ImGui::InputText("Rename", nameBuffer, 512, ImGuiInputTextFlags_EnterReturnsTrue)) {
			std::string name(nameBuffer);
			gameObject->SetName(name);
			memset(nameBuffer, 0, 512);
			ImGui::CloseCurrentPopup();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Remove")) {
			if (std::find(objectsToRemove_.begin(), objectsToRemove_.end(), gameObject) == objectsToRemove_.end())
				objectsToRemove_.push_back(gameObject);
		}

		ImGui::EndPopup();
	} else {
		memset(nameBuffer, 0, 128);
	}
	ImGui::PopID();
}

void ZHierarchyTool::SelectObjectsIfClicked(std::shared_ptr<ZGameObject> &gameObject) {
	if (ImGui::IsItemClicked()) {
		if (!zenith::Input()->Key(ZKEY_LEFT_CONTROL) && !zenith::Input()->Key(ZKEY_RIGHT_CONTROL)) {
			editor_->SelectedObjects().clear();
		}
		editor_->SelectedObjects()[gameObject->ID()] = gameObject;
	}
}

void ZHierarchyTool::HandleDragDrop(std::shared_ptr<ZGameObject> &gameObject, std::string &tooltipText) {
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ZGameObject")) {
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

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("ZGameObject", gameObject.get(), sizeof(ZGameObject));
		ImGui::Text("%s", tooltipText.c_str());
		ImGui::EndDragDropSource();
	}
}

void ZHierarchyTool::ReparentObjects() {
    for (auto pair : parentObjectPairs_) {
        if (pair.second->Parent() == pair.first.get()) {    // When trying to parent a parent into one of the children, swap their positions in the heirarchy
            pair.first->RemoveChild(pair.second);
            pair.first->Parent()->AddChild(pair.second);
            pair.second->AddChild(pair.first);
        } else {
            pair.second->AddChild(pair.first);
        }
    }
    parentObjectPairs_.clear();
}

void ZHierarchyTool::ProcessPendingRemovals() {
	if (!objectsToRemove_.empty()) 
		editor_->SelectedObjects().clear();
	for (auto object : objectsToRemove_)
		object->Destroy();
	objectsToRemove_.clear();
}

void ZHierarchyTool::End() {
    ReparentObjects();
	ProcessPendingRemovals();
	ImGui::End();
}
