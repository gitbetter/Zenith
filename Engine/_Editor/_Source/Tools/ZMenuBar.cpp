/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZMenuBar.cpp
 
    Created by Adrian Sanchez on 23/05/19.
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

#include "ZMenuBar.hpp"
#include "ZEditor.hpp"
#include "ZGame.hpp"
#include "ZScene.hpp"
#include "ZGameObject.hpp"

void ZMenuBar::Begin() {
	ImGui::BeginMainMenuBar();
}

void ZMenuBar::Update() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::BeginMenu("New")) {
			ImGui::MenuItem("Project");
			ImGui::MenuItem("Scene");
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Scene")) {
		if (ImGui::BeginMenu("Create")) {
			if (ImGui::BeginMenu("Game Object")) {
				if (ImGui::MenuItem("Empty")) {
					std::shared_ptr<ZGameObject> empty = editor_->ObjectTemplates()["ZGO_EMPTY"]->Clone();
					zenith::Game()->ActiveScene()->AddGameObject(empty);
				}
				if (ImGui::MenuItem("Cube")) {
					std::shared_ptr<ZGameObject> cube = editor_->ObjectTemplates()["ZGO_CUBE"]->Clone();
					zenith::Game()->ActiveScene()->AddGameObject(cube);
				}
				if (ImGui::MenuItem("Sphere")) {
					std::shared_ptr<ZGameObject> sphere = editor_->ObjectTemplates()["ZGO_SPHERE"]->Clone();
					zenith::Game()->ActiveScene()->AddGameObject(sphere);
				}
				if (ImGui::MenuItem("Plane")) {
					std::shared_ptr<ZGameObject> plane = editor_->ObjectTemplates()["ZGO_PLANE"]->Clone();
					zenith::Game()->ActiveScene()->AddGameObject(plane);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Camera")) {
				std::shared_ptr<ZGameObject> camera = editor_->ObjectTemplates()["ZGO_CAMERA"]->Clone();
				zenith::Game()->ActiveScene()->AddGameObject(camera);
			}
			if (ImGui::MenuItem("Light")) {
				std::shared_ptr<ZGameObject> light = editor_->ObjectTemplates()["ZLT_LIGHT"]->Clone();
				zenith::Game()->ActiveScene()->AddGameObject(light);
			}
			if (ImGui::MenuItem("Trigger Volume")) {
				std::shared_ptr<ZGameObject> trigger = editor_->ObjectTemplates()["ZTR_TRIGGER"]->Clone();
				zenith::Game()->ActiveScene()->AddGameObject(trigger);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
		ImGui::EndMenu();
	}
}

void ZMenuBar::End() {
	ImGui::EndMainMenuBar();
}