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
#include "ZEditor.hpp"
#include "ZDomain.hpp"
#include "ZInput.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZCameraComponent.hpp"
#include "ZAnimatorComponent.hpp"
#include "ZScriptComponent.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZEventAgent.hpp"
#include "IconsFontAwesome5.h"

void ZInspectorTool::Begin() {
	ImGui::Begin(name_.c_str());
}

void ZInspectorTool::Initialize() {
    ZEventDelegate resourceLoadedDelegate = fastdelegate::MakeDelegate(this, &ZInspectorTool::HandleTextureLoaded);
    zenith::EventAgent()->AddListener(resourceLoadedDelegate, ZTextureReadyEvent::Type);

    zenith::Graphics()->Strategy()->LoadTextureAsync(OBJECT_CUBE_IMAGE_PATH, "");
    memset(objectNameBuffer_, 0, 512);
}

void ZInspectorTool::Update() {
	std::shared_ptr<ZGameObject> selectedObject;
	if (editor_->SelectedObjects().size() == 1)
		selectedObject = editor_->SelectedObjects().begin()->second;

    ImGui::Image((ImTextureID)objectCubeImage_.id, ImVec2(40.0, 40.0));
    ImGui::SameLine();
	DrawNameField(selectedObject);
	DrawTransformProperties(selectedObject);

	if (selectedObject) {
		DrawComponentProperties(selectedObject);
		lastSelectedObject_ = selectedObject->ID();
	}
}

void ZInspectorTool::DrawNameField(std::shared_ptr<ZGameObject> &selectedObject) {
	if (selectedObject) {
		if (strcmp(objectNameBuffer_, selectedObject->Name().c_str()) != 0) {
			if (selectedObject->Name().empty()) memset(objectNameBuffer_, 0, 512);
			else memcpy(objectNameBuffer_, selectedObject->Name().c_str(), selectedObject->Name().size());
		}
	}

	ImGui::SetCursorPos(ImVec2(60.0, 37.0));
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, selectedObject == nullptr);
	if (ImGui::InputText("##name", objectNameBuffer_, 512, ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (selectedObject) {
			std::string name(objectNameBuffer_);
			selectedObject->SetName(name);
		}
	}
	ImGui::PopItemFlag();
}

void ZInspectorTool::DrawTransformProperties(std::shared_ptr<ZGameObject> &selectedObject) {
	ImGui::PushFont(editor_->HeaderFont());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));
	ImGui::Text("%s", "Transform");
	ImGui::PopStyleVar();
	ImGui::PopFont();

	RedoButton(selectedObject, "Transform");

	static glm::vec3 position(0.f), rotation(0.f), scale(0.f);
	static bool dragActive = false;

	if (selectedObject && lastSelectedObject_ != selectedObject->ID()) {
		position = selectedObject->Position();
		rotation = glm::eulerAngles(selectedObject->Orientation() * radsToAngleRatio_);
		scale = selectedObject->Scale();
	}

	if (ImGui::DragFloat3("Position", (float*)glm::value_ptr(position), 0.1f)) {
		dragActive = true;
		if (selectedObject) selectedObject->SetPosition(position);
	}
	if (RedoButton(selectedObject, "Position")) {
		position = glm::vec3(0.f);
	}
	if (ImGui::DragFloat3("Rotation", (float*)glm::value_ptr(rotation), 0.1f)) {
		dragActive = true;
		if (selectedObject) selectedObject->SetOrientation(rotation * angleToRadsRatio_);
	}
	if (RedoButton(selectedObject, "Rotation")) {
		rotation = glm::vec3(0.f);
	}
	if (ImGui::DragFloat3("Scale", (float*)glm::value_ptr(scale), 0.1f)) {
		dragActive = true;
		if (selectedObject) selectedObject->SetScale(scale);
	}
	if (RedoButton(selectedObject, "Scale")) {
		scale = glm::vec3(1.f);
	}

	if (dragActive && !zenith::Input()->Mouse(ZMOUSE_LEFT)) {
		ReleaseMouse(); dragActive = false;
	} else if (dragActive) {
		CaptureMouse();
	}
}

void ZInspectorTool::DrawComponentProperties(std::shared_ptr<ZGameObject>& selectedObject) {
	if (selectedObject->FindComponent<ZGraphicsComponent>()) {
		if (ImGui::CollapsingHeader("Graphics Component", ImGuiTreeNodeFlags_DefaultOpen)) {
			
		}
	}
	if (selectedObject->FindComponent<ZPhysicsComponent>()) {
		if (ImGui::CollapsingHeader("Physics Component", ImGuiTreeNodeFlags_DefaultOpen)) {

		}
	}
	if (selectedObject->FindComponent<ZCameraComponent>()) {
		if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen)) {

		}
	} 
	if (selectedObject->FindComponent<ZAnimatorComponent>()) {
		if (ImGui::CollapsingHeader("Animator Component", ImGuiTreeNodeFlags_DefaultOpen)) {

		}
	} 
	if (selectedObject->FindComponent<ZScriptComponent>()) {
		if (ImGui::CollapsingHeader("Script Component", ImGuiTreeNodeFlags_DefaultOpen)) {

		}
	}
}

void ZInspectorTool::CaptureMouse() {
	if (zenith::Domain()->Strategy()->IsCursorCaptured()) return;

	double x, y;
	zenith::Input()->GetCursorPosition(x, y);
	zenith::Domain()->Strategy()->CaptureCursor();
	zenith::Input()->SetCursorPosition(x, y);
}

void ZInspectorTool::ReleaseMouse() {
	if (!zenith::Domain()->Strategy()->IsCursorCaptured()) return;
	zenith::Domain()->Strategy()->ReleaseCursor();
}

bool ZInspectorTool::RedoButton(std::shared_ptr<ZGameObject>& selectedObject, const std::string& prop) {
	bool clicked = false;
	std::string buttonID(ICON_FA_REDO "##" + prop);
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 15);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, selectedObject == nullptr);
	if (ImGui::Button(buttonID.c_str())) {
		clicked = true;
		if (prop == "Transform") {
			selectedObject->SetPosition(glm::vec3(0.f));
			selectedObject->SetOrientation(glm::vec3(0.f));
			selectedObject->SetScale(glm::vec3(1.f));
		} else if (prop == "Position") {
			selectedObject->SetPosition(glm::vec3(0.f));
		} else if (prop == "Rotation") {
			selectedObject->SetOrientation(glm::vec3(0.f));
		} else if (prop == "Scale") {
			selectedObject->SetScale(glm::vec3(1.f));
		}
	}
	ImGui::PopItemFlag();
	ImGui::PopStyleColor(2);
	return clicked;
}

void ZInspectorTool::End() {
	ImGui::End();
}

void ZInspectorTool::HandleTextureLoaded(std::shared_ptr<ZEvent> event) {
    std::shared_ptr<ZTextureReadyEvent> textureEvent = std::static_pointer_cast<ZTextureReadyEvent>(event);
    if (textureEvent->Texture().path == OBJECT_CUBE_IMAGE_PATH) {
        objectCubeImage_ = textureEvent->Texture();
        
        ZEventDelegate resourceLoadedDelegate = fastdelegate::MakeDelegate(this, &ZInspectorTool::HandleTextureLoaded);
        zenith::EventAgent()->RemoveListener(resourceLoadedDelegate, ZTextureReadyEvent::Type);
    }
}
